import sys
import time
import signal
import tempfile
import subprocess
import pyarrow

from prettytable import PrettyTable
from pyarrow import flight
from tqdm import tqdm

# Configuration
MODELAR_GITHUB_REPO = "https://github.com/ModelarData/ModelarDB-RS"
TABLE_NAME = "evaluate"
STDOUT = subprocess.PIPE
STDERR = subprocess.PIPE
DATA_SET_LIST = {"AP": "Air-pressure.csv", "BT": "Basel-temp.csv",
                 "BW": "Basel-wind.csv", "CDT": "Chengdu-traj.csv",
                 "CT": "City-temp.csv", "DT": "Dew-point-temp.csv",
                 "IR": "IR-bio-temp.csv", "MT": "Motor-temp.csv",
                 "PM10": "PM10-dust.csv", "SG": "Smart-grid.csv",
                 "SUSA": "Stocks-USA.csv", "TD": "T-drive.csv",
                 "WS": "Wind-Speed.csv"}
DATA_SET_LIST_TSBS = {"TLat": "Tsbs-iot-latitude.csv",
                      "TLng": "Tsbs-iot-longitude.csv"}
ERROR_BOUND_LIST = [0.1, 0.001, 0.001]
BLOCK_SIZE_LIST = [1000, 5000, 10000, 20000, 50000, 100000]
BLOCK_SIZE_LIST_TSBS = [230000]
DATA_SET_PREFIX = "../test/data_set/"


# Helper Functions.
def git_clone(url):
    subprocess.run(["git", "clone", url], stdout=STDOUT, stderr=STDERR)


def cargo_build_release(modelardb_folder):
    process = subprocess.run(
        ["cargo", "build", "--release"],
        cwd=modelardb_folder,
        stdout=STDOUT,
        stderr=STDERR,
    )

    return process.returncode == 0


def start_modelardbd(modelardb_folder, data_folder):
    process = subprocess.Popen(
        ["target/release/modelardbd", data_folder],
        cwd=modelardb_folder,
        stdout=STDOUT,
        stderr=STDERR,
    )

    # Ensure process is fully started.
    while not b"Starting Apache Arrow Flight on" in process.stdout.readline():
        time.sleep(1)

    return process


def errors_occurred(output_stream):
    normalized = output_stream.lower()
    return b"error" in normalized or b"panicked" in normalized


def print_stream(output_stream):
    print()
    print(output_stream.decode("utf-8"))


def create_model_table(flient_client, table_name, error_bound):
    sql = f"CREATE MODEL TABLE {
        table_name} (ts TIMESTAMP, va FIELD({error_bound}))"
    ticket = flight.Ticket(str.encode(sql))
    result = flient_client.do_get(ticket)
    return list(result)


def ingest_test_data(test_data, error_bound):
    flight_client = flight.FlightClient("grpc://127.0.0.1:9999")
    create_table_sql = f"CREATE MODEL TABLE test (ts TIMESTAMP, va FIELD({
        str(error_bound)}))"
    ticket = flight.Ticket(create_table_sql)
    flight_client.do_get(ticket)
    ts = pyarrow.array([100 * i for i in range(len(test_data))])  # linear ts
    va = pyarrow.array(test_data, type=pyarrow.float32())
    table = pyarrow.Table.from_arrays([ts, va], names=["ts", "va"])
    upload_descriptor = flight.FlightDescriptor.for_path("test")
    writer, _ = flight_client.do_put(upload_descriptor, table.schema)
    writer.write(table)
    writer.close()
    flight_client.close()


def measure_data_folder_size_in_kib(data_folder):
    du_output = subprocess.check_output(["du", "-k", "-d0", data_folder])
    return int(du_output.split(b"\t")[0])


def send_sigint_to_process(process):
    process.send_signal(signal.SIGINT)

    # Ensure process is fully shutdown.
    while process.poll() is None:
        time.sleep(0.001)

    process.wait()

    stderr = process.stderr.read()
    if errors_occurred(stderr):
        print_stream(stderr)
        return True


def read_blocks(file_path, each_block_size):
    with open(file_path, 'r+', encoding='utf-8') as file:
        block = []
        for line in file:
            block.append(float(line.rstrip()))
            if len(block) == each_block_size:
                yield block
                block = []
        if block:
            yield block


# Main Function.
if __name__ == "__main__":
    # The script assumes it runs on Linux.
    if sys.platform != "linux":
        print(f"ERROR: {sys.argv[0]} only supports Linux")
        sys.exit(1)

    git_clone(MODELAR_GITHUB_REPO)

    modelardb_folder = "ModelarDB-RS"

    print("Start building ModelarDB-RS...")
    # Prepare new executable.
    if not cargo_build_release(modelardb_folder):
        raise ValueError("Failed to build ModelarDB in release mode.")
    print("Success in building ModelarDB-RS.")

    print("Starting evaluating...")
    result_table = PrettyTable()
    fields = ["BlockSize / DataSet"]
    fields += [alias for alias, file_path in DATA_SET_LIST.items()]
    print(fields)
    result_table.field_names = fields
    # Evaluate compression
    for block_size in BLOCK_SIZE_LIST:
        this_row = [block_size]
        for error_bound in ERROR_BOUND_LIST:
            for alias, file_path in DATA_SET_LIST.items():
                block_count = 0
                compressed_file_size_in_bytes = 0
                # Prepare data folder.
                # Load each block
                for block in tqdm(read_blocks(DATA_SET_PREFIX + file_path, block_size)):
                    if len(block) != block_size:
                        break
                    temporary_directory = tempfile.TemporaryDirectory()
                    data_folder = temporary_directory.name
                    block_count += 1
                    modelardbd = start_modelardbd(
                        modelardb_folder, data_folder)
                    ingest_test_data(block, error_bound)
                    send_sigint_to_process(modelardbd)  # Flush.
                    compressed_file_size_in_bytes += measure_data_folder_size_in_kib(
                        data_folder) * 1024
                this_row.append(compressed_file_size_in_bytes /
                                (block_count * block_size * 4))
        result_table.add_row(this_row)
    print(result_table)
    with open("results.csv", "w+", encoding="utf-8") as csv_file:
        csv_file.write(result_table.get_csv_string())
