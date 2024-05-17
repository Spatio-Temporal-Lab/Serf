#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "serf/compressor/SerfXORCompressor.h"
#include "serf/decompressor/SerfXORDecompressor.h"
#include "serf/compressor/SerfQtCompressor.h"
#include "serf/decompressor/SerfQtDecompressor.h"
#include "deflate/DeflateCompressor.h"
#include "deflate/DeflateDecompressor.h"
#include "lz4/LZ4Compressor.h"
#include "lz4/LZ4Decompressor.h"
#include "fpc/FpcCompressor.h"
#include "fpc/FpcDecompressor.h"
#include "alp/include/alp.hpp"
#include "chimp/ChimpCompressor.h"
#include "chimp/ChimpDecompressor.h"
#include "elf/elf.h"
#include "gorilla/gorilla_compressor.h"
#include "gorilla/gorilla_decompressor.h"
#include "buff/buff_compressor.h"
#include "buff/buff_decompressor.h"
#include "lz77/fastlz.h"
#include "lzw/src/LZW.h"
#include "machete/machete.h"
#include "sz/sz/include/sz.h"


const static size_t kBlockSize = 1000;
const static size_t kDoubleSize = 64;
const static std::string kExportExprTablePrefix = "../../test/";
const static std::string kExportExprTableFileName = "perf_table.csv";
const static std::string kDataSetDirPrefix = "../../test/dataSet/";
const static std::string kDataSetList[] = {
        "init.csv",
        "Air-pressure.csv",
        "Air-sensor.csv",
        "Bird-migration.csv",
        "Bitcoin-price.csv",
        "Basel-temp.csv",
        "Basel-wind.csv",
        "City-temp.csv",
        "Dew-point-temp.csv",
        "IR-bio-temp.csv",
        "PM10-dust.csv",
        "Stocks-DE.csv",
        "Stocks-UK.csv",
        "Stocks-USA.csv",
        "Wind-Speed.csv"
};
const static std::unordered_map<std::string, int> kFileNameToAdjustDigit{
        {"init.csv",           0},
        {"Air-pressure.csv",   0},
        {"Air-sensor.csv",     128},
        {"Bird-migration.csv", 60},
        {"Bitcoin-price.csv",  511220},
        {"Basel-temp.csv",     77},
        {"Basel-wind.csv",     128},
        {"City-temp.csv",      355},
        {"Dew-point-temp.csv", 94},
        {"IR-bio-temp.csv",    49},
        {"PM10-dust.csv",      256},
        {"Stocks-DE.csv",      253},
        {"Stocks-UK.csv",      8047},
        {"Stocks-USA.csv",     243},
        {"Wind-Speed.csv",     2}
};
//constexpr static double kMaxDiffList[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};
constexpr static double kMaxDiffList[] = {1.0E-4};
//constexpr static int kBlockSizeList[] = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

static std::string double_to_string_with_precision(double val, size_t precision) {
    std::ostringstream stringBuffer;
    stringBuffer << std::fixed << std::setprecision(precision) << val;
    return stringBuffer.str();
}


class PerfRecord {
public:
    PerfRecord() = default;

    void IncreaseCompressionTime(std::chrono::microseconds &duration) {
        compression_time_ += duration;
    }

    auto &compression_time() {
        return compression_time_;
    }

    auto AvgCompressionTimePerBlock() {
        return (double) compression_time_.count() / block_count_;
    }

    void IncreaseDecompressionTime(std::chrono::microseconds &duration) {
        decompression_time_ += duration;
    }

    auto &decompression_time() {
        return decompression_time_;
    }

    auto AvgDecompressionTimePerBlock() {
        return (double) decompression_time_.count() / block_count_;
    }

    long compressed_size_in_bits() {
        return compressed_size_in_bits_;
    }

    void AddCompressedSize(long size) {
        compressed_size_in_bits_ += size;
    }

    void set_block_count(int blockCount_) {
        block_count_ = blockCount_;
    }

    int block_count() {
        return block_count_;
    }

    double CalCompressionRatio() {
        return (double) compressed_size_in_bits_ / (double) (block_count_ * kBlockSize * kDoubleSize);
    }

private:
    std::chrono::microseconds compression_time_ = std::chrono::microseconds::zero();
    std::chrono::microseconds decompression_time_ = std::chrono::microseconds::zero();
    long compressed_size_in_bits_ = 0;
    int block_count_ = 0;
};


class ExprConf {
public:
    struct hash {
        std::size_t operator()(const ExprConf &conf) const {
            return std::hash<std::string>()(conf.method_ + conf.data_set_ + conf.max_diff_);
        }
    };

    ExprConf() = delete;

    ExprConf(std::string method, std::string data_set, double max_diff) : method_(std::move(method)),
                                                                          data_set_(std::move(data_set)),
                                                                          max_diff_(double_to_string_with_precision(
                                                                                  max_diff, 8)) {}

    bool operator==(const ExprConf &otherConf) const {
        return method_ == otherConf.method_ && data_set_ == otherConf.data_set_ && max_diff_ == otherConf.max_diff_;
    }

    std::string method() const {
        return method_;
    }

    std::string data_set() const {
        return data_set_;
    }

    std::string max_diff() const {
        return max_diff_;
    }

private:
    const std::string method_;
    const std::string data_set_;
    const std::string max_diff_;
};


std::unordered_map<ExprConf, PerfRecord, ExprConf::hash> expr_table;

void ExportTotalExprTable() {
    std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
    if (!expr_table_output_stream.is_open()) {
        std::cerr << "Failed to export performance data." << std::endl;
        exit(-1);
    }
    // Write header
    expr_table_output_stream
            << "Method,DataSet,MaxDiff,BlockCount,CompressionRatio,CompressionTime(AvgPerBlock),DecompressionTime(AvgPerBlock)"
            << std::endl;
    // Write record
    for (const auto &conf_record: expr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.max_diff() << ","
                                 << record.block_count() << ","
                                 << record.CalCompressionRatio() << "," << record.AvgCompressionTimePerBlock() << ","
                                 << record.AvgDecompressionTimePerBlock() << std::endl;
    }
    // Go!!
    expr_table_output_stream.flush();
    expr_table_output_stream.close();
}

void ExportExprTableWithCompressionRatio() {
    std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
    if (!expr_table_output_stream.is_open()) {
        std::cerr << "Failed to export performance data." << std::endl;
        exit(-1);
    }
    // Write header
    expr_table_output_stream
            << "Method,DataSet,MaxDiff,CompressionRatio"
            << std::endl;
    // Write record
    for (const auto &conf_record: expr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.max_diff() << ","
                                 << record.CalCompressionRatio() << std::endl;
    }
    // Go!!
    expr_table_output_stream.flush();
    expr_table_output_stream.close();
}

void ExportExprTableWithCompressionRatioNoSpecificDataset() {
    std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
    if (!expr_table_output_stream.is_open()) {
        std::cerr << "Failed to export performance data." << std::endl;
        exit(-1);
    }
    // Write header
    expr_table_output_stream
            << "Method,MaxDiff,CompressionRatio"
            << std::endl;
    // Aggregate data and write
    std::unordered_map<ExprConf, PerfRecord, ExprConf::hash> aggr_table;
    for (const auto &conf_record: expr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        auto aggr_key = ExprConf(conf.method(), "", std::stod(conf.max_diff()));
        auto find_result = aggr_table.find(aggr_key);
        if (find_result != aggr_table.end()) {
            auto &selected_record = find_result->second;
            selected_record.set_block_count(selected_record.block_count() + record.block_count());
            selected_record.AddCompressedSize(record.compressed_size_in_bits());
        } else {
            PerfRecord new_record;
            new_record.set_block_count(record.block_count());
            new_record.AddCompressedSize(record.compressed_size_in_bits());
            aggr_table.insert(std::make_pair(aggr_key, new_record));
        }
    }
    for (const auto &conf_record: aggr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        expr_table_output_stream << conf.method() << "," << conf.max_diff() << "," << record.CalCompressionRatio() << std::endl;
    }
    // Go!!
    expr_table_output_stream.flush();
    expr_table_output_stream.close();
}

void ExportExprTableWithCompressionTime() {
    std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
    if (!expr_table_output_stream.is_open()) {
        std::cerr << "Failed to export performance data." << std::endl;
        exit(-1);
    }
    // Write header
    expr_table_output_stream
            << "Method,DataSet,MaxDiff,CompressionTime(Total)"
            << std::endl;
    // Write record
    for (const auto &conf_record: expr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.max_diff() << ","
                                 << record.compression_time().count() << std::endl;
    }
    // Go!!
    expr_table_output_stream.flush();
    expr_table_output_stream.close();
}

void ExportExprTableWithCompressionTimeNoSpecificDataset() {
    std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
    if (!expr_table_output_stream.is_open()) {
        std::cerr << "Failed to export performance data." << std::endl;
        exit(-1);
    }
    // Write header
    expr_table_output_stream
            << "Method,MaxDiff,CompressionTime(AvgPerBlock)"
            << std::endl;
    // Aggregate data and write
    std::unordered_map<ExprConf, PerfRecord, ExprConf::hash> aggr_table;
    for (const auto &conf_record: expr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        auto aggr_key = ExprConf(conf.method(), "", std::stod(conf.max_diff()));
        auto find_result = aggr_table.find(aggr_key);
        if (find_result != aggr_table.end()) {
            auto &selected_record = find_result->second;
            selected_record.set_block_count(selected_record.block_count() + record.block_count());
            selected_record.IncreaseCompressionTime(record.compression_time());
        } else {
            PerfRecord new_record;
            new_record.set_block_count(record.block_count());
            new_record.IncreaseCompressionTime(record.compression_time());
            aggr_table.insert(std::make_pair(aggr_key, new_record));
        }
    }
    for (const auto &conf_record: aggr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        expr_table_output_stream << conf.method() << "," << conf.max_diff() << "," << std::fixed << std::setprecision(6) << record.AvgCompressionTimePerBlock() << std::endl;
    }
    // Go!!
    expr_table_output_stream.flush();
    expr_table_output_stream.close();
}

void ExportExprTableWithDecompressionTime() {
    std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
    if (!expr_table_output_stream.is_open()) {
        std::cerr << "Failed to export performance data." << std::endl;
        exit(-1);
    }
    // Write header
    expr_table_output_stream
            << "Method,DataSet,MaxDiff,DecompressionTime(Total)"
            << std::endl;
    // Write record
    for (const auto &conf_record: expr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.max_diff() << ","
                                 << record.decompression_time().count() << std::endl;
    }
    // Go!!
    expr_table_output_stream.flush();
    expr_table_output_stream.close();
}

void ExportExprTableWithDecompressionTimeNoSpecificDataset() {
    std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
    if (!expr_table_output_stream.is_open()) {
        std::cerr << "Failed to export performance data." << std::endl;
        exit(-1);
    }
    // Write header
    expr_table_output_stream
            << "Method,MaxDiff,DecompressionTime(AvgPerBlock)"
            << std::endl;
    // Aggregate data and write
    std::unordered_map<ExprConf, PerfRecord, ExprConf::hash> aggr_table;
    for (const auto &conf_record: expr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        auto aggr_key = ExprConf(conf.method(), "", std::stod(conf.max_diff()));
        auto find_result = aggr_table.find(aggr_key);
        if (find_result != aggr_table.end()) {
            auto &selected_record = find_result->second;
            selected_record.set_block_count(selected_record.block_count() + record.block_count());
            selected_record.IncreaseDecompressionTime(record.decompression_time());
        } else {
            PerfRecord new_record;
            new_record.set_block_count(record.block_count());
            new_record.IncreaseDecompressionTime(record.decompression_time());
            aggr_table.insert(std::make_pair(aggr_key, new_record));
        }
    }
    for (const auto &conf_record: aggr_table) {
        auto conf = conf_record.first;
        auto record = conf_record.second;
        expr_table_output_stream << conf.method() << "," << conf.max_diff() << "," << std::fixed << std::setprecision(6) << record.AvgDecompressionTimePerBlock() << std::endl;
    }
    // Go!!
    expr_table_output_stream.flush();
    expr_table_output_stream.close();
}

std::vector<double> ReadBlock(std::ifstream &file_input_stream_ref) {
    std::vector<double> ret;
    int entry_count = 0;
    double buffer;
    while (!file_input_stream_ref.eof() && entry_count < kBlockSize) {
        file_input_stream_ref >> buffer;
        ret.emplace_back(buffer);
        ++entry_count;
    }
    return ret;
}

std::vector<double> ReadBlockWithPrecisionInfo(std::ifstream &file_input_stream_ref, int *precision) {
    std::vector<double> return_block;
    int read_double_count = 0;
    std::string double_value_raw_string;
    size_t max_precision = 0;
    size_t cur_precision;
    while (!file_input_stream_ref.eof() && read_double_count < kBlockSize) {
        std::getline(file_input_stream_ref, double_value_raw_string);
        if (double_value_raw_string.empty()) continue;
        if (double_value_raw_string.find('.') == std::string::npos) {
            cur_precision = 1;
            max_precision = std::max(max_precision, cur_precision);
        } else {
            cur_precision = double_value_raw_string.size() - double_value_raw_string.find('.') - 1;
            max_precision = std::max(max_precision, cur_precision);
        }
        double buffer = std::stod(double_value_raw_string);
        return_block.emplace_back(buffer);
        ++read_double_count;
    }
    *precision = (int) max_precision;
    return return_block;
}

std::vector<std::string> ReadRawBlock(std::ifstream &file_input_stream_ref) {
    std::vector<std::string> ret;
    int read_string_count = 0;
    std::string buffer;
    while (!file_input_stream_ref.eof() && read_string_count < kBlockSize) {
        std::getline(file_input_stream_ref, buffer);
        if (buffer.empty()) continue;
        ret.emplace_back(buffer);
        ++read_string_count;
    }
    return ret;
}

void ResetFileStream(std::ifstream &data_set_input_stream_ref) {
    data_set_input_stream_ref.clear();
    data_set_input_stream_ref.seekg(0, std::ios::beg);
}

PerfRecord PerfSerfXOR(std::ifstream &data_set_input_stream_ref, double max_diff, const std::string &data_set) {
    PerfRecord perf_record;
    SerfXORCompressor serf_xor_compressor(kBlockSize, max_diff, kFileNameToAdjustDigit.find(data_set)->second);
    SerfXORDecompressor serf_xor_decompressor(kFileNameToAdjustDigit.find(data_set)->second);

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            serf_xor_compressor.AddValue(value);
        }
        serf_xor_compressor.Close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(serf_xor_compressor.compressed_size_in_bits());
        Array<uint8_t> compression_output = serf_xor_compressor.compressed_bytes();
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = serf_xor_decompressor.decompress(compression_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfSerfQt(std::ifstream &data_set_input_stream_ref, double max_diff) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        SerfQtCompressor serf_qt_compressor(kBlockSize, max_diff);
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            serf_qt_compressor.AddValue(value);
        }
        serf_qt_compressor.Close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(serf_qt_compressor.get_compressed_size_in_bits());
        Array<uint8_t> compression_output = serf_qt_compressor.GetBytes();
        SerfQtDecompressor serf_qt_decompressor(compression_output);
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = serf_qt_decompressor.Decompress();
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfDeflate(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        DeflateCompressor deflate_compressor;
        DeflateDecompressor deflate_decompressor;
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            deflate_compressor.addValue(value);
        }
        deflate_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(deflate_compressor.getCompressedSizeInBits());
        Array<uint8_t> compression_output = deflate_compressor.getBytes();
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = deflate_decompressor.decompress(compression_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfLZ4(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        LZ4Compressor lz_4_compressor;
        LZ4Decompressor lz_4_decompressor;
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            lz_4_compressor.addValue(value);
        }
        lz_4_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(lz_4_compressor.getCompressedSizeInBits());
        Array<char> compression_output = lz_4_compressor.getBytes();
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = lz_4_decompressor.decompress(compression_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfFPC(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        FpcCompressor fpc_compressor(5, kBlockSize);
        FpcDecompressor fpc_decompressor(5, kBlockSize);
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            fpc_compressor.addValue(value);
        }
        fpc_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(fpc_compressor.getCompressedSizeInBits());
        std::vector<char> compression_output = fpc_compressor.getBytes();
        fpc_decompressor.setBytes(compression_output.data(), compression_output.size());
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompressed_data = fpc_decompressor.decompress();
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfALP(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        uint8_t compression_output_buffer[(kBlockSize * sizeof(double)) + 8096];
        auto decompressed_buffer_size = alp::AlpApiUtils::align_value<size_t, alp::config::VECTOR_SIZE>(kBlockSize);
        double decompress_output[decompressed_buffer_size];
        alp::AlpCompressor alp_compressor;
        alp::AlpDecompressor alp_decompressor;
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        alp_compressor.compress(original_data.data(), original_data.size(), compression_output_buffer);
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(alp_compressor.get_size() * 8);
        auto decompression_start_time = std::chrono::steady_clock::now();
        alp_decompressor.decompress(compression_output_buffer, kBlockSize, decompress_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfElf(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        uint8_t *compression_output_buffer;
        double decompression_output[kBlockSize];
        ssize_t compression_output_len_in_bytes;
        ssize_t decompression_len;
        ++block_count;

        auto compression_start_time = std::chrono::steady_clock::now();
        compression_output_len_in_bytes = elf_encode(original_data.data(), original_data.size(),
                                                     &compression_output_buffer, 0);
        auto compression_end_time = std::chrono::steady_clock::now();

        perf_record.AddCompressedSize(compression_output_len_in_bytes * 8);

        auto decompression_start_time = std::chrono::steady_clock::now();
        decompression_len = elf_decode(compression_output_buffer, compression_output_len_in_bytes, decompression_output,
                                       0);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfChimp128(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        ChimpCompressor chimp_compressor(128);
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            chimp_compressor.addValue(value);
        }
        chimp_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(chimp_compressor.get_size());
        Array<uint8_t> compression_output = chimp_compressor.get_compress_pack();
        auto decompression_start_time = std::chrono::steady_clock::now();
        ChimpDecompressor chimp_decompressor(compression_output, 128);
        std::vector<double> decompression_output = chimp_decompressor.decompress();
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfGorilla(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        GorillaCompressor gorilla_compressor(kBlockSize);
        GorillaDecompressor gorilla_decompressor;
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        for (const auto &value: original_data) {
            gorilla_compressor.addValue(value);
        }
        gorilla_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(gorilla_compressor.get_compress_size_in_bits());
        Array<uint8_t> compression_output = gorilla_compressor.get_compress_pack();
        auto decompression_start_time = std::chrono::steady_clock::now();
        std::vector<double> decompression_output = gorilla_decompressor.decompress(compression_output);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfBuff(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    int max_precision_this_block;
    while ((original_data = ReadBlockWithPrecisionInfo(data_set_input_stream_ref, &max_precision_this_block)).size() ==
           kBlockSize) {
        ++block_count;
        Array<double> original_data_array(kBlockSize);
        for (int i = 0; i < kBlockSize; ++i) {
            original_data_array[i] = original_data[i];
        }
        BuffCompressor buff_compressor(kBlockSize, max_precision_this_block);
        auto compression_start_time = std::chrono::steady_clock::now();
        buff_compressor.compress(original_data_array);
        buff_compressor.close();
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(buff_compressor.get_size());
        Array<uint8_t> compression_output = buff_compressor.get_out();
        BuffDecompressor buff_decompressor(compression_output);
        auto decompression_start_time = std::chrono::steady_clock::now();
        Array<double> decompression_output = buff_decompressor.decompress();
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfLZ77(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        auto *compression_output = new uint8_t[10000];
        auto *decompression_output = new double[1000];
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        int compression_output_len = fastlz_compress_level(2, original_data.data(), kBlockSize * sizeof(double),
                                                           compression_output);
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(compression_output_len * 8);
        auto decompression_start_time = std::chrono::steady_clock::now();
        fastlz_decompress(compression_output, compression_output_len, decompression_output,
                          kBlockSize * sizeof(double));
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);

        delete[] compression_output;
        delete[] decompression_output;
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfMachete(std::ifstream &data_set_input_stream_ref, double max_diff) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        ++block_count;
        auto *compression_buffer = new uint8_t[100000];
        auto *decompression_buffer = new double[kBlockSize];
        auto compression_start_time = std::chrono::steady_clock::now();
        ssize_t compression_output_len = machete_compress<lorenzo1, hybrid>(original_data.data(), original_data.size(),
                                                                            &compression_buffer, max_diff);
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(compression_output_len * 8);
        auto decompression_start_time = std::chrono::steady_clock::now();
        ssize_t decompression_output_len = machete_decompress<lorenzo1, hybrid>(compression_buffer,
                                                                                compression_output_len,
                                                                                decompression_buffer);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);

        delete[] compression_buffer;
        delete[] decompression_buffer;
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfSZ(std::ifstream &data_set_input_stream_ref, double max_diff) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<double> original_data;
    while ((original_data = ReadBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        ++block_count;
        size_t compression_output_len;
        auto decompression_output = new double[kBlockSize];
        auto compression_start_time = std::chrono::steady_clock::now();
        auto compression_output = SZ_compress_args(SZ_DOUBLE, original_data.data(), &compression_output_len,
                                                   ABS, max_diff * 0.99, 0, 0, 0, 0, 0, 0, original_data.size());
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(compression_output_len * 8);
        auto decompression_start_time = std::chrono::steady_clock::now();
        size_t decompression_output_len = SZ_decompress_args(SZ_DOUBLE, compression_output,
                                                             compression_output_len, decompression_output, 0, 0,
                                                             0, 0, kBlockSize);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);

        delete[] decompression_output;
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

PerfRecord PerfLZW(std::ifstream &data_set_input_stream_ref) {
    PerfRecord perf_record;

    int block_count = 0;
    std::vector<std::string> original_data;
    while ((original_data = ReadRawBlock(data_set_input_stream_ref)).size() == kBlockSize) {
        std::string input_string;
        for (const auto &item: original_data) input_string += (item + " ");
        LZW *lzw = LZW::instance();
        ++block_count;
        auto compression_start_time = std::chrono::steady_clock::now();
        auto [compression_result, compression_result_code] = lzw->Compress(input_string);
        EXPECT_EQ(compression_result_code, LZWResultCode::LZW_OK);
        auto compression_end_time = std::chrono::steady_clock::now();
        perf_record.AddCompressedSize(compression_result.size() * 8);
        auto decompression_start_time = std::chrono::steady_clock::now();
        auto [decompression_result, decompression_result_code] = lzw->Decompress(compression_result);
        EXPECT_EQ(decompression_result_code, LZWResultCode::LZW_OK);
        auto decompression_end_time = std::chrono::steady_clock::now();

        auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                compression_end_time - compression_start_time);
        auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
                decompression_end_time - decompression_start_time);

        perf_record.IncreaseCompressionTime(compression_time_in_a_block);
        perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
    }

    perf_record.set_block_count(block_count);
    return perf_record;
}

TEST(Perf, All) {
    for (const auto &data_set: kDataSetList) {
        std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
        if (!data_set_input_stream.is_open()) {
            std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
        }

        // Lossy
        for (const auto &max_diff: kMaxDiffList) {
            expr_table.insert(std::make_pair(ExprConf("SerfXOR", data_set, max_diff),
                                             PerfSerfXOR(data_set_input_stream, max_diff, data_set)));
            ResetFileStream(data_set_input_stream);
            expr_table.insert(std::make_pair(ExprConf("SerfQt", data_set, max_diff),
                                             PerfSerfQt(data_set_input_stream, max_diff)));
            ResetFileStream(data_set_input_stream);
            expr_table.insert(std::make_pair(ExprConf("Machete", data_set, max_diff),
                                             PerfMachete(data_set_input_stream, max_diff)));
            ResetFileStream(data_set_input_stream);
            expr_table.insert(
                    std::make_pair(ExprConf("SZ", data_set, max_diff), PerfSZ(data_set_input_stream, max_diff)));
            ResetFileStream(data_set_input_stream);
        }

        // Lossless
//        expr_table.insert(std::make_pair(ExprConf("ALP", data_set, 0), PerfALP(data_set_input_stream)));
//        ResetFileStream(data_set_input_stream);
//        expr_table.insert(std::make_pair(ExprConf("Buff", data_set, 0), PerfBuff(data_set_input_stream)));
//        ResetFileStream(data_set_input_stream);
//        expr_table.insert(std::make_pair(ExprConf("Chimp", data_set, 0), PerfChimp128(data_set_input_stream)));
//        ResetFileStream(data_set_input_stream);
//        expr_table.insert(std::make_pair(ExprConf("Deflate", data_set, 0), PerfDeflate(data_set_input_stream)));
//        ResetFileStream(data_set_input_stream);
//        expr_table.insert(std::make_pair(ExprConf("Elf", data_set, 0), PerfElf(data_set_input_stream)));
//        ResetFileStream(data_set_input_stream);
//        expr_table.insert(std::make_pair(ExprConf("FPC", data_set, 0), PerfFPC(data_set_input_stream)));
//        ResetFileStream(data_set_input_stream);
//        expr_table.insert(std::make_pair(ExprConf("Gorilla", data_set, 0), PerfGorilla(data_set_input_stream)));
//        ResetFileStream(data_set_input_stream);
//        expr_table.insert(std::make_pair(ExprConf("LZ4", data_set, 0), PerfLZ4(data_set_input_stream)));
//        ResetFileStream(data_set_input_stream);
//        expr_table.insert(std::make_pair(ExprConf("LZ77", data_set, 0), PerfLZ77(data_set_input_stream)));
//        ResetFileStream(data_set_input_stream);

        data_set_input_stream.close();
    }

    // Export all performance data
//    ExportTotalExprTable();
//    ExportExprTableWithCompressionRatioNoSpecificDataset();
//    ExportExprTableWithCompressionTimeNoSpecificDataset();
    ExportExprTableWithDecompressionTimeNoSpecificDataset();
}