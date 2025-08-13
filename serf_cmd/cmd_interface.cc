#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <cmath>

#include "compressor/serf_xor_compressor.h"
#include "decompressor/serf_xor_decompressor.h"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <mode> <compression_error_bound> <input_file_path> <output_file_path>" << std::endl;
        std::cerr << "Modes: compress | decompress" << std::endl;
        std::cerr << "Example (compress): " << argv[0] << " compress 0.01 input.txt output.sfz" << std::endl;
        std::cerr << "Example (decompress): " << argv[0] << " decompress 0.01 input.sfz output.txt" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    float compression_error_bound = std::atof(argv[2]);
    std::string input_file_path = argv[3];
    std::string output_file_path = argv[4];

    std::ifstream input_file(input_file_path, std::ios::binary);
    if (!input_file) {
        std::cerr << "Error: Cannot open input file: " << input_file_path << "\n";
        return 1;
    }

    std::ofstream output_file(output_file_path, std::ios::binary);
    if (!output_file) {
        std::cerr << "Error: Cannot open output file: " << output_file_path << "\n";
        return 1;
    }

    if (mode == "compress") {
        std::cout << "Compression mode selected.\n";
        std::cout << "Compression error bound: " << compression_error_bound << "\n";
        std::cout << "Input file: " << input_file_path << "\n";
        std::cout << "Output file: " << output_file_path << "\n";

        SerfXORCompressor xor_compressor(1000, compression_error_bound, 0);
        std::string line;
        while (std::getline(input_file, line)) {
            try {
                double value = std::stof(line);
                xor_compressor.AddValue(value);
            } catch (const std::exception& e) {
                std::cerr << "Error processing line: " << line << " (" << e.what() << ")\n";
                return 1;
            }
        }
        xor_compressor.Close();

        auto compressed_bytes = xor_compressor.compressed_bytes_last_block();
        output_file.write(reinterpret_cast<const char*>(compressed_bytes.begin()), compressed_bytes.length());

        std::cout << "Compression completed successfully.\n";
        std::cout << "Compressed size of last block: " << xor_compressor.compressed_size_last_block() << " bytes\n";
    } else if (mode == "decompress") {
        std::cout << "Decompression mode selected.\n";
        std::cout << "Input file: " << input_file_path << "\n";
        std::cout << "Output file: " << output_file_path << "\n";

        SerfXORDecompressor xor_decompressor(0);
        std::vector<uint8_t> compressed_data((std::istreambuf_iterator<char>(input_file)),
                                             std::istreambuf_iterator<char>());
        Array<uint8_t> compressed_bytes(compressed_data);
        const auto& decompressed_values = xor_decompressor.Decompress(compressed_bytes);

        for (const auto& value : decompressed_values) {
            output_file << value << "\n";
        }

        std::cout << "Decompression completed successfully.\n";
    } else {
        std::cerr << "Error: Invalid mode. Use 'compress' or 'decompress'.\n";
        return 1;
    }

    // Close files
    input_file.close();
    output_file.close();

    return 0;
}