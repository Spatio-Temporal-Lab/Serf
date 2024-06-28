#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <utility>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "../src/compressor/serf_xor_compressor.h"
#include "../src/decompressor/serf_xor_decompressor.h"
#include "../src/compressor/serf_qt_compressor.h"
#include "../src/decompressor/serf_qt_decompressor.h"
#include "../src/compressor_32/serf_xor_compressor_32.h"
#include "../src/decompressor_32/serf_xor_decompressor_32.h"
#include "../src/compressor_32/serf_qt_compressor_32.h"
#include "../src/decompressor_32/serf_qt_decompressor_32.h"

#include "baselines/deflate/DeflateCompressor.h"
#include "baselines/deflate/DeflateDecompressor.h"

#include "baselines/lz4/LZ4Compressor.h"
#include "baselines/lz4/LZ4Decompressor.h"

#include "baselines/fpc/FpcCompressor.h"
#include "baselines/fpc/FpcDecompressor.h"

#include "baselines/alp/include/alp.hpp"

#include "baselines/chimp128/ChimpCompressor.h"
#include "baselines/chimp128/ChimpDecompressor.h"

#include "baselines/elf/elf.h"

#include "baselines/gorilla/gorilla_compressor.h"
#include "baselines/gorilla/gorilla_decompressor.h"

#include "baselines/buff/buff_compressor.h"
#include "baselines/buff/buff_decompressor.h"

#include "baselines/lz77/fastlz.h"

#include "baselines/lzw/src/LZW.h"

#include "baselines/machete/machete.h"

#include "baselines/sz3/include/SZ3/api/sz.hpp"

#include "baselines/sz_adt/sz/inc/sz.h"

#include "baselines/zstd/lib/zstd.h"

#include "baselines/snappy/snappy.h"

#include "baselines/sim_piece/sim_piece.h"

// Remember to change this if you run single precision experiment
const static size_t kDoubleSize = 64;
const static std::string kExportExprTablePrefix = "../../test/";
const static std::string kExportExprTableFileName = "perf_table.csv";
const static std::string kDataSetDirPrefix = "../../test/data_set/";
const static std::string kDataSetList[] = {
    "Air-pressure.csv",
    "Air-sensor.csv",
    "Bird-migration.csv",
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
const static std::unordered_map<std::string, std::string> kAbbrToDataList {
    {"AP", "Air-pressure.csv"},
    {"AS", "Air-sensor.csv"},
    {"BM", "Bird-migration.csv"},
    {"BT", "Basel-temp.csv"},
    {"BW", "Basel-wind.csv"},
    {"CT", "City-temp.csv"},
    {"DT", "Dew-point-temp.csv"},
    {"IR", "IR-bio-temp.csv"},
    {"PM10", "PM10-dust.csv"},
    {"SDE", "Stocks-DE.csv"},
    {"SUK", "Stocks-UK.csv"},
    {"SUSA", "Stocks-USA.csv"},
    {"WS", "Wind-Speed.csv"}
};
const static std::string kMethodList[] = {
    "LZ77", "Zstd", "Snappy", "SZ3", "Machete", "SimPiece", "Buff", "Deflate", "LZ4", "FPC", "Gorilla", "Chimp128",
    "Elf", "SerfQt", "SerfXOR"
};
const static std::string kAbbrList[] = {
    "AP", "AS", "BM", "BT", "BW", "CT", "DT", "IR", "PM10", "SDE", "SUK", "SUSA", "WS"
};
const static std::string kDataSetList32[] = {
    "City-temp.csv",
    "Dew-point-temp.csv",
    "IR-bio-temp.csv",
    "PM10-dust.csv",
    "Wind-Speed.csv"
};
const static std::unordered_map<std::string, int> kFileNameToAdjustDigit{
    {"Air-pressure.csv", 0},
    {"Air-sensor.csv", 128},
    {"Bird-migration.csv", 60},
    {"Bitcoin-price.csv", 511220},
    {"Basel-temp.csv", 77},
    {"Basel-wind.csv", 128},
    {"City-temp.csv", 355},
    {"Dew-point-temp.csv", 94},
    {"IR-bio-temp.csv", 49},
    {"PM10-dust.csv", 256},
    {"Stocks-DE.csv", 253},
    {"Stocks-UK.csv", 8047},
    {"Stocks-USA.csv", 243},
    {"Wind-Speed.csv", 2}
};
//constexpr static int kBlockSizeList[] = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
constexpr static int kBlockSizeList[] = {50};
//constexpr static double kMaxDiffList[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};
constexpr static float kMaxDiffList[] = {1.0E-3};
//constexpr static int kBlockSizeList[] = {50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};

static int global_block_size = 0;

std::vector<double> ReadBlock(std::ifstream &file_input_stream_ref, int block_size) {
  std::vector<double> ret;
  ret.reserve(block_size);
  int entry_count = 0;
  double buffer;
  while (!file_input_stream_ref.eof() && entry_count < block_size) {
    file_input_stream_ref >> buffer;
    ret.emplace_back(buffer);
    ++entry_count;
  }
  return ret;
}

std::vector<float> ReadBlock32(std::ifstream &file_input_stream_ref, int block_size) {
  std::vector<float> ret;
  ret.reserve(block_size);
  int entry_count = 0;
  float buffer;
  while (!file_input_stream_ref.eof() && entry_count < block_size) {
    file_input_stream_ref >> buffer;
    ret.emplace_back(buffer);
    ++entry_count;
  }
  return ret;
}

std::vector<std::string> ReadRawBlock(std::ifstream &file_input_stream_ref, int block_size) {
  std::vector<std::string> ret;
  int read_string_count = 0;
  std::string buffer;
  while (!file_input_stream_ref.eof() && read_string_count < block_size) {
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

static std::string double_to_string_with_precision(double val, size_t precision) {
  std::ostringstream stringBuffer;
  stringBuffer << std::fixed << std::setprecision(precision) << val;
  return stringBuffer.str();
}

static int GetAlpha(double max_diff) {
  double max_diff_to_alpha_table[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};
  for (int i = 0; i < 8; ++i)
    if (max_diff == max_diff_to_alpha_table[i])
      return i + 1;
  return -1;
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
    return static_cast<double>(compression_time_.count()) / block_count_;
  }

  void IncreaseDecompressionTime(std::chrono::microseconds &duration) {
    decompression_time_ += duration;
  }

  auto &decompression_time() {
    return decompression_time_;
  }

  auto AvgDecompressionTimePerBlock() {
    return static_cast<double>(decompression_time_.count()) / block_count_;
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
    return (double) compressed_size_in_bits_ / (double) (block_count_ * global_block_size * kDoubleSize);
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

  ExprConf(std::string method, std::string data_set, double max_diff) : method_(method),
                                                                        data_set_(data_set),
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
      << "Method,DataSet,MaxDiff,CompressionRatio,CompressionTime(AvgPerBlock),DecompressionTime(AvgPerBlock)"
      << std::endl;
  // Write record
  for (const auto &conf_record : expr_table) {
    auto conf = conf_record.first;
    auto record = conf_record.second;
    expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.max_diff() << ","
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
  for (const auto &conf_record : expr_table) {
    auto conf = conf_record.first;
    auto record = conf_record.second;
    expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.max_diff() << ","
                             << record.CalCompressionRatio() << std::endl;
  }
  // Go!!
  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void ExportExprTableWithCompressionRatioAvg() {
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
  for (const auto &conf_record : expr_table) {
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
  for (const auto &conf_record : aggr_table) {
    auto conf = conf_record.first;
    auto record = conf_record.second;
    expr_table_output_stream << conf.method() << "," << conf.max_diff() << "," << record.CalCompressionRatio()
                             << std::endl;
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
  for (const auto &conf_record : expr_table) {
    auto conf = conf_record.first;
    auto record = conf_record.second;
    expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.max_diff() << ","
                             << record.compression_time().count() << std::endl;
  }
  // Go!!
  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void ExportExprTableWithCompressionTimeAvg() {
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
  for (const auto &conf_record : expr_table) {
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
  for (const auto &conf_record : aggr_table) {
    auto conf = conf_record.first;
    auto record = conf_record.second;
    expr_table_output_stream << conf.method() << "," << conf.max_diff() << "," << std::fixed << std::setprecision(6)
                             << record.AvgCompressionTimePerBlock() << std::endl;
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
  for (const auto &conf_record : expr_table) {
    auto conf = conf_record.first;
    auto record = conf_record.second;
    expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.max_diff() << ","
                             << record.decompression_time().count() << std::endl;
  }
  // Go!!
  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void ExportExprTableWithDecompressionTimeAvg() {
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
  for (const auto &conf_record : expr_table) {
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
  for (const auto &conf_record : aggr_table) {
    auto conf = conf_record.first;
    auto record = conf_record.second;
    expr_table_output_stream << conf.method() << "," << conf.max_diff() << "," << std::fixed << std::setprecision(6)
                             << record.AvgDecompressionTimePerBlock() << std::endl;
  }
  // Go!!
  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

// Auto-Gen
void GenTableCT() {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodList) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set_abbr : kAbbrList) {
      expr_table_output_stream << expr_table.find(ExprConf(method, kAbbrToDataList.find(data_set_abbr)->second,
                                                           kMaxDiffList[0]))->second.AvgCompressionTimePerBlock() <<
                                                           ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenTableCR() {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }
  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodList) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set_abbr : kAbbrList) {
      expr_table_output_stream << expr_table.find(ExprConf(method, kAbbrToDataList.find(data_set_abbr)
      ->second, kMaxDiffList[0]))->second.CalCompressionRatio() << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenTableDT() {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + kExportExprTableFileName);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }
  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodList) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set_abbr : kAbbrList) {
      expr_table_output_stream << expr_table.find(ExprConf(method, kAbbrToDataList.find(data_set_abbr)->second,
                                                           kMaxDiffList[0]))->second.AvgDecompressionTimePerBlock() <<
                               ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

PerfRecord PerfSerfXOR(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                       const std::string &data_set) {
  PerfRecord perf_record;

  SerfXORCompressor serf_xor_compressor(1000, max_diff, kFileNameToAdjustDigit.find(data_set)->second);
  SerfXORDecompressor serf_xor_decompressor(kFileNameToAdjustDigit.find(data_set)->second);

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) serf_xor_compressor.AddValue(value);
    serf_xor_compressor.Close();
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(serf_xor_compressor.compressed_size_last_block());
    Array<uint8_t> compression_output = serf_xor_compressor.compressed_bytes_last_block();

    auto decompression_start_time = std::chrono::steady_clock::now();
    std::vector<double> decompressed_data = serf_xor_decompressor.Decompress(compression_output);
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

PerfRecord PerfSerfQt(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size) {
  PerfRecord perf_record;

  SerfQtCompressor serf_qt_compressor(block_size, max_diff);
  SerfQtDecompressor serf_qt_decompressor;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) serf_qt_compressor.AddValue(value);
    serf_qt_compressor.Close();
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(serf_qt_compressor.get_compressed_size_in_bits());
    Array<uint8_t> compression_output = serf_qt_compressor.compressed_bytes();

    auto decompression_start_time = std::chrono::steady_clock::now();
    std::vector<double> decompressed_data = serf_qt_decompressor.Decompress(compression_output);
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

PerfRecord PerfDeflate(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    DeflateCompressor deflate_compressor(block_size);
    DeflateDecompressor deflate_decompressor;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) deflate_compressor.addValue(value);
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

PerfRecord PerfLZ4(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    LZ4Compressor lz_4_compressor(block_size);
    LZ4Decompressor lz_4_decompressor;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) lz_4_compressor.addValue(value);
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

PerfRecord PerfFPC(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    FpcCompressor fpc_compressor(5, block_size);
    FpcDecompressor fpc_decompressor(5, block_size);

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) fpc_compressor.addValue(value);
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

PerfRecord PerfALP(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    uint8_t compression_output_buffer[(block_size * sizeof(double)) + 1024];
    auto decompressed_buffer_size = alp::AlpApiUtils::align_value<size_t, alp::config::VECTOR_SIZE>(block_size);
    double decompress_output[decompressed_buffer_size];
    alp::AlpCompressor alp_compressor;
    alp::AlpDecompressor alp_decompressor;

    auto compression_start_time = std::chrono::steady_clock::now();
    alp_compressor.compress(original_data.data(), original_data.size(), compression_output_buffer);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(alp_compressor.get_size() * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    alp_decompressor.decompress(compression_output_buffer, block_size, decompress_output);
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

PerfRecord PerfZstd(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    char compression_output[block_size * 10];
    double decompression_output[block_size];

    auto compression_start_time = std::chrono::steady_clock::now();
    size_t compression_output_len = ZSTD_compress(compression_output, block_size * 10, original_data.data(),
                                                  original_data.size() * 8, 3);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    ZSTD_decompress(decompression_output, block_size * 8, compression_output, compression_output_len);
    auto decompression_end_time = std::chrono::steady_clock::now();

    auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        compression_end_time - compression_start_time);
    auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        decompression_end_time - decompression_start_time);

    perf_record.IncreaseCompressionTime(compression_time_in_a_block);
    perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);

    for (int i = 0; i < block_size; ++i) {
      EXPECT_FLOAT_EQ(original_data[i], decompression_output[i]);
    }
  }

  perf_record.set_block_count(block_count);
  return perf_record;
}

PerfRecord PerfSnappy(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    std::string compression_output;
    std::string decompression_output;
    auto compression_start_time = std::chrono::steady_clock::now();
    size_t compression_output_len = snappy::Compress(reinterpret_cast<const char *>(original_data.data()),
                                                   original_data.size() * 8, &compression_output);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    snappy::Uncompress(compression_output.data(), compression_output.size(), &decompression_output);
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

PerfRecord PerfElf(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    uint8_t *compression_output_buffer;
    double *decompression_output = new double[block_size];
    ssize_t compression_output_len_in_bytes;
    ssize_t decompression_len;

    auto compression_start_time = std::chrono::steady_clock::now();
    compression_output_len_in_bytes = elf_encode(original_data.data(), original_data.size(),
                                                 &compression_output_buffer, 0);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len_in_bytes * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    decompression_len = elf_decode(compression_output_buffer, compression_output_len_in_bytes, decompression_output,
                                   0);
    delete[] decompression_output;
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

PerfRecord PerfChimp128(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    ChimpCompressor chimp_compressor(128);
    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) {
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

PerfRecord PerfGorilla(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;
  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    GorillaCompressor gorilla_compressor(block_size);
    GorillaDecompressor gorilla_decompressor;
    ++block_count;
    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) {
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

PerfRecord PerfBuff(std::ifstream &data_set_input_stream_ref, int alpha, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    Array<double> original_data_array(block_size);
    for (int i = 0; i < block_size; ++i) {
      original_data_array[i] = original_data[i];
    }
    BuffCompressor buff_compressor(block_size, alpha);

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

PerfRecord PerfLZ77(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;
  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    auto *compression_output = new uint8_t[10000];
    auto *decompression_output = new double[1000];

    auto compression_start_time = std::chrono::steady_clock::now();
    int compression_output_len = fastlz_compress_level(2, original_data.data(), block_size * sizeof(double),
                                                       compression_output);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    fastlz_decompress(compression_output, compression_output_len, decompression_output,
                      block_size * sizeof(double));
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

PerfRecord PerfMachete(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    auto *compression_buffer = new uint8_t[100000];
    auto *decompression_buffer = new double[block_size];

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

PerfRecord PerfSZ3(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    SZ3::Config conf(block_size);
    conf.cmprAlgo = SZ3::ALGO_INTERP_LORENZO;
    conf.errorBoundMode = SZ3::EB_ABS;
    conf.absErrorBound = max_diff;
    size_t compression_output_len;

    auto compression_start_time = std::chrono::steady_clock::now();
    char *compression_output = SZ_compress<double>(conf, original_data.data(), compression_output_len);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    auto *decompression_output = SZ_decompress<double>(conf, compression_output, compression_output_len);
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

PerfRecord PerfSZ_ADT(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  // sz init
  if (confparams_cpr == nullptr) confparams_cpr = (sz_params*) malloc(sizeof(sz_params));
  if (exe_params == nullptr) exe_params = (sz_exedata*) malloc(sizeof(sz_exedata));
  setDefaulParams(exe_params, confparams_cpr);
  confparams_cpr->errorBoundMode = SZ_ABS;
  confparams_cpr->absErrBoundDouble = max_diff * 0.999;
  confparams_cpr->ifAdtFse = 1;
  confparams_cpr->sampleDistance = 10;
  auto *compression_output = new unsigned char [block_size * 8];
  auto *decompression_output = new double [block_size];

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    sz_params comp_params = *confparams_cpr;
    auto compression_start_time = std::chrono::steady_clock::now();
    size_t compression_output_len = SZ_compress_args(SZ_DOUBLE, original_data.data(), original_data.size(),
                                                   compression_output, &comp_params);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    size_t decompression_out_size = SZ_decompress(SZ_DOUBLE, compression_output, compression_output_len, block_size,
                                                  (unsigned char* ) decompression_output);
    auto decompression_end_time = std::chrono::steady_clock::now();

    auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        compression_end_time - compression_start_time);
    auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        decompression_end_time - decompression_start_time);

    perf_record.IncreaseCompressionTime(compression_time_in_a_block);
    perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
  }

  // sz free and exit
  if(confparams_cpr!= nullptr) {
    free(confparams_cpr);
    confparams_cpr = nullptr;
  }
  if(exe_params != nullptr) {
    free(exe_params);
    exe_params = nullptr;
  }
  delete[] compression_output;
  delete[] decompression_output;

  perf_record.set_block_count(block_count);
  return perf_record;
}

PerfRecord PerfLZW(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<std::string> original_data;
  while ((original_data = ReadRawBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    std::string input_string;
    for (const auto &item : original_data) input_string += (item + " ");
    LZW *lzw = LZW::instance();
    ++block_count;

    auto compression_start_time = std::chrono::steady_clock::now();
    auto [compression_result, compression_result_code] = lzw->Compress(input_string);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_result.size() * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    auto [decompression_result, decompression_result_code] = lzw->Decompress(compression_result);
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

PerfRecord PerfSimPiece(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    std::vector<Point> input_points;
    for (int i = 0; i < original_data.size(); ++i) {
      input_points.emplace_back(i, original_data[i]);
    }

    char *compression_output = new char [original_data.size() * 8];
    int compression_output_len = 0;
    int timestamp_store_size;
    auto compression_start_time = std::chrono::steady_clock::now();
    SimPiece sim_piece_compress(input_points, max_diff);
    compression_output_len = sim_piece_compress.toByteArray(compression_output, true, &timestamp_store_size);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize((compression_output_len - timestamp_store_size) * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    SimPiece sim_piece_decompress(compression_output, compression_output_len, true);
    sim_piece_decompress.decompress();
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

// Single Precision
PerfRecord PerfSerfXOR_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size) {
  PerfRecord perf_record;

  SerfXORCompressor32 serf_xor_compressor(block_size, max_diff);
  SerfXORDecompressor32 serf_xor_decompressor;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) serf_xor_compressor.AddValue(value);
    serf_xor_compressor.Close();
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(serf_xor_compressor.compressed_size_in_bits());
    Array<uint8_t> compression_output = serf_xor_compressor.compressed_bytes();

    auto decompression_start_time = std::chrono::steady_clock::now();
    std::vector<float> decompressed_data = serf_xor_decompressor.Decompress(compression_output);
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

PerfRecord PerfSerfQt_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size) {
  PerfRecord perf_record;

  SerfQtCompressor32 serf_qt_compressor(block_size, max_diff);
  SerfQtDecompressor32 serf_qt_decompressor;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) serf_qt_compressor.AddValue(value);
    serf_qt_compressor.Close();
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(serf_qt_compressor.stored_compressed_size_in_bits());
    Array<uint8_t> compression_output = serf_qt_compressor.compressed_bytes();

    auto decompression_start_time = std::chrono::steady_clock::now();
    std::vector<float> decompressed_data = serf_qt_decompressor.Decompress(compression_output);
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

PerfRecord PerfDeflate_32(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    DeflateCompressor deflate_compressor(block_size);
    DeflateDecompressor deflate_decompressor;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) deflate_compressor.addValue32(value);
    deflate_compressor.close();
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(deflate_compressor.getCompressedSizeInBits());
    Array<uint8_t> compression_output = deflate_compressor.getBytes();

    auto decompression_start_time = std::chrono::steady_clock::now();
    std::vector<float> decompressed_data = deflate_decompressor.decompress32(compression_output);
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

PerfRecord PerfLZ4_32(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    LZ4Compressor lz_4_compressor(block_size);
    LZ4Decompressor lz_4_decompressor;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) lz_4_compressor.addValue32(value);
    lz_4_compressor.close();
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(lz_4_compressor.getCompressedSizeInBits());
    Array<char> compression_output = lz_4_compressor.getBytes();

    auto decompression_start_time = std::chrono::steady_clock::now();
    std::vector<float> decompressed_data = lz_4_decompressor.decompress32(compression_output);
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

PerfRecord PerfLZ77_32(std::ifstream &data_set_input_stream_ref, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<float> original_data;
  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    auto *compression_output = new uint8_t[10000];
    auto *decompression_output = new float[1000];

    auto compression_start_time = std::chrono::steady_clock::now();
    int compression_output_len = fastlz_compress_level(2, original_data.data(), block_size * sizeof(float),
                                                       compression_output);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    fastlz_decompress(compression_output, compression_output_len, decompression_output,
                      block_size * sizeof(float));
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

PerfRecord PerfSZ3_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    SZ3::Config conf(block_size);
    conf.cmprAlgo = SZ3::ALGO_INTERP_LORENZO;
    conf.errorBoundMode = SZ3::EB_ABS;
    conf.absErrorBound = max_diff;
    size_t compression_output_len;

    auto compression_start_time = std::chrono::steady_clock::now();
    char *compression_output = SZ_compress<float>(conf, original_data.data(), compression_output_len);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    auto *decompression_output = SZ_decompress<float>(conf, compression_output, compression_output_len);
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

TEST(Perf, All) {
  global_block_size = kBlockSizeList[0];
  for (const auto &data_set : kDataSetList) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    // Lossy
    for (const auto &max_diff : kMaxDiffList) {
      expr_table.insert(std::make_pair(ExprConf("SerfXOR", data_set, max_diff), PerfSerfXOR(data_set_input_stream,
                                                                                            max_diff,
                                                                                            global_block_size,
                                                                                            data_set)));
      ResetFileStream(data_set_input_stream);
      expr_table.insert(std::make_pair(ExprConf("SerfQt", data_set, max_diff), PerfSerfQt(data_set_input_stream,
                                                                                          max_diff,
                                                                                          global_block_size)));
      ResetFileStream(data_set_input_stream);
      expr_table.insert(std::make_pair(ExprConf("Machete", data_set, max_diff), PerfMachete(data_set_input_stream,
                                                                                            max_diff,
                                                                                            global_block_size)));
      ResetFileStream(data_set_input_stream);
      expr_table.insert(std::make_pair(ExprConf("SZ3", data_set, max_diff), PerfSZ3(data_set_input_stream, max_diff,
                                                                                   global_block_size)));
      ResetFileStream(data_set_input_stream);
      expr_table.insert(std::make_pair(ExprConf("Buff", data_set, max_diff), PerfBuff(data_set_input_stream,
                                                                                      GetAlpha(max_diff),
                                                                                      global_block_size)));
      ResetFileStream(data_set_input_stream);
      expr_table.insert(std::make_pair(ExprConf("SimPiece", data_set, max_diff), PerfSimPiece(data_set_input_stream,
                                                                                      max_diff,
                                                                                      global_block_size)));
      ResetFileStream(data_set_input_stream);
    }

    // Lossless
    expr_table.insert(std::make_pair(ExprConf("Chimp128", data_set, kMaxDiffList[0]), PerfChimp128
    (data_set_input_stream, global_block_size)));
    ResetFileStream(data_set_input_stream);
    expr_table.insert(std::make_pair(ExprConf("Deflate", data_set, kMaxDiffList[0]), PerfDeflate(data_set_input_stream,
                                                                                   global_block_size)));
    ResetFileStream(data_set_input_stream);
    expr_table.insert(std::make_pair(ExprConf("Elf", data_set, kMaxDiffList[0]), PerfElf(data_set_input_stream, global_block_size)));
    ResetFileStream(data_set_input_stream);
    expr_table.insert(std::make_pair(ExprConf("FPC", data_set, kMaxDiffList[0]), PerfFPC(data_set_input_stream, global_block_size)));
    ResetFileStream(data_set_input_stream);
    expr_table.insert(std::make_pair(ExprConf("Gorilla", data_set, kMaxDiffList[0]), PerfGorilla(data_set_input_stream,
                                                                                   global_block_size)));
    ResetFileStream(data_set_input_stream);
    expr_table.insert(std::make_pair(ExprConf("LZ4", data_set, kMaxDiffList[0]), PerfLZ4(data_set_input_stream, global_block_size)));
    ResetFileStream(data_set_input_stream);
    expr_table.insert(std::make_pair(ExprConf("LZ77", data_set, kMaxDiffList[0]), PerfLZ77(data_set_input_stream, global_block_size)));
    ResetFileStream(data_set_input_stream);
    expr_table.insert(std::make_pair(ExprConf("Zstd", data_set, kMaxDiffList[0]), PerfZstd(data_set_input_stream,
                                                                                        global_block_size)));
    ResetFileStream(data_set_input_stream);
    expr_table.insert(std::make_pair(ExprConf("Snappy", data_set, kMaxDiffList[0]), PerfSnappy(data_set_input_stream,
                                                                                           global_block_size)));
    ResetFileStream(data_set_input_stream);

    data_set_input_stream.close();
  }

//    ExportTotalExprTable();
//    ExportExprTableWithCompressionRatioAvg();
//    ExportExprTableWithCompressionTimeAvg();
//    ExportExprTableWithDecompressionTimeAvg();
    GenTableDT();
}
