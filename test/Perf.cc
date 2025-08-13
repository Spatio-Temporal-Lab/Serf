#include <gtest/gtest.h>

#include "Perf_baseline_inc.hpp"
#include "Perf_expr_config.hpp"
#include "Perf_file_utils.hpp"
#include "Perf_expr_data_struct.hpp"

void ExportTotalExprTable(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "total" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  // Write header
  expr_table_output_stream
      << "Method,DataSet,BlockSize,MaxDiff,CompressionRatio,CompressionTime(AvgPerBlock),DecompressionTime(AvgPerBlock)"
      << std::endl;
  // Write record
  for (const auto &conf_record : expr_table) {
    auto conf = conf_record.first;
    auto record = conf_record.second;
    expr_table_output_stream << conf.method() << "," << conf.data_set() << "," << conf.block_size() << ","
                             << conf.max_diff() << "," << record.CalCompressionRatio(conf) << ","
                             << record.AvgCompressionTimePerBlock() << ","
                             << record.AvgDecompressionTimePerBlock() << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

// Auto-Gen for the Overall Experiment

void GenOverallTableCR(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "overall_cr" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodListOverall) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetList) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSizeOverall, kMaxDiffOverall);
      expr_table_output_stream << expr_table.find(this_conf)->second.CalCompressionRatio(this_conf) << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenOverallTableCT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "overall_ct" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodListOverall) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetList) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSizeOverall, kMaxDiffOverall);
      expr_table_output_stream << expr_table.find(this_conf)->second.AvgCompressionTimePerBlock() << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenOverallTableDT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "overall_dt" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodListOverall) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetList) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSizeOverall, kMaxDiffOverall);
      expr_table_output_stream << expr_table.find(this_conf)->second.AvgDecompressionTimePerBlock() << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

// Auto-Gen for the SinglePrecision Experiment

void GenSinglePrecisionTableCR(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "single_precision_cr" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodList32) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetList32) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSize32, kMaxDiff32, true);
      expr_table_output_stream << expr_table.find(this_conf)->second.CalCompressionRatio(this_conf) << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenSinglePrecisionTableCT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "single_precision_ct" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodList32) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetList32) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSize32, kMaxDiff32, true);
      expr_table_output_stream << expr_table.find(this_conf)->second.AvgCompressionTimePerBlock() << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenSinglePrecisionTableDT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "single_precision_dt" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodList32) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetList32) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSize32, kMaxDiff32, true);
      expr_table_output_stream << expr_table.find(this_conf)->second.AvgDecompressionTimePerBlock() << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

// Auto-Gen for the TSBS Experiment

void GenTSBSTableCR(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "tsbs_cr" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodListTSBS) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetListTSBS) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSizeTSBS, kMaxDiffTSBS);
      expr_table_output_stream << expr_table.find(this_conf)->second.CalCompressionRatio(this_conf) << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenTSBSTableCT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "tsbs_ct" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodListTSBS) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetListTSBS) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSizeTSBS, kMaxDiffTSBS);
      expr_table_output_stream << expr_table.find(this_conf)->second.AvgCompressionTimePerBlock() << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenTSBSTableDT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "tsbs_dt" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodListTSBS) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetListTSBS) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSizeTSBS, kMaxDiffTSBS);
      expr_table_output_stream << expr_table.find(this_conf)->second.AvgDecompressionTimePerBlock() << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

// Auto-Gen for the Param(Abs MaxDiff) Experiment

void GenParamAbsDiffTable(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "param_abs_diff_results" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &max_diff : kMaxDiffList) {
    expr_table_output_stream << max_diff << std::endl;
    expr_table_output_stream << "Compression Ratio" << std::endl;
    for (const auto &method : kMethodListParamAbsMaxDiff) {
      expr_table_output_stream << method << ",";
      for (const auto &data_set : kDataSetList) {
        ExprConf this_conf = ExprConf(method, data_set, kBlockSizeParamAbsMaxDiff, max_diff);
        auto result = expr_table.find(this_conf);
        if (result != expr_table.end()) {
          expr_table_output_stream << result->second.CalCompressionRatio(this_conf) << ",";
        }
      }
      expr_table_output_stream << std::endl;
    }
    expr_table_output_stream << "Compression Time" << std::endl;
    for (const auto &method : kMethodListParamAbsMaxDiff) {
      expr_table_output_stream << method << ",";
      for (const auto &data_set : kDataSetList) {
        ExprConf this_conf = ExprConf(method, data_set, kBlockSizeParamAbsMaxDiff, max_diff);
        auto result = expr_table.find(this_conf);
        if (result != expr_table.end()) {
          expr_table_output_stream << result->second.AvgCompressionTimePerBlock() << ",";
        }
      }
      expr_table_output_stream << std::endl;
    }
    expr_table_output_stream << "Decompression Time" << std::endl;
    for (const auto &method : kMethodListParamAbsMaxDiff) {
      expr_table_output_stream << method << ",";
      for (const auto &data_set : kDataSetList) {
        ExprConf this_conf = ExprConf(method, data_set, kBlockSizeParamAbsMaxDiff, max_diff);
        auto result = expr_table.find(this_conf);
        if (result != expr_table.end()) {
          expr_table_output_stream << result->second.AvgDecompressionTimePerBlock() << ",";
        }
      }
      expr_table_output_stream << std::endl;
    }
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

// Auto-Gen for the Param(Block Size) Experiment

void GenParamBlockSizeTable(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "param_block_size_results" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &block_size : kBlockSizeList) {
    expr_table_output_stream << block_size << std::endl;
    expr_table_output_stream << "Compression Ratio" << std::endl;
    for (const auto &method : kMethodListParamBlockSize) {
      expr_table_output_stream << method << ",";
      for (const auto &data_set : kDataSetList) {
        ExprConf this_conf = ExprConf(method, data_set, block_size, kAbsMaxDiffParamBlockSize);
        auto result = expr_table.find(this_conf);
        if (result != expr_table.end()) {
          expr_table_output_stream << result->second.CalCompressionRatio(this_conf) << ",";
        }
      }
      expr_table_output_stream << std::endl;
    }
    expr_table_output_stream << "Compression Time" << std::endl;
    for (const auto &method : kMethodListParamBlockSize) {
      expr_table_output_stream << method << ",";
      for (const auto &data_set : kDataSetList) {
        ExprConf this_conf = ExprConf(method, data_set, block_size, kAbsMaxDiffParamBlockSize);
        auto result = expr_table.find(this_conf);
        if (result != expr_table.end()) {
          expr_table_output_stream << result->second.AvgCompressionTimePerBlock() << ",";
        }
      }
      expr_table_output_stream << std::endl;
    }
    expr_table_output_stream << "Decompression Time" << std::endl;
    for (const auto &method : kMethodListParamBlockSize) {
      expr_table_output_stream << method << ",";
      for (const auto &data_set : kDataSetList) {
        ExprConf this_conf = ExprConf(method, data_set, block_size, kAbsMaxDiffParamBlockSize);
        auto result = expr_table.find(this_conf);
        if (result != expr_table.end()) {
          expr_table_output_stream << result->second.AvgDecompressionTimePerBlock() << ",";
        }
      }
      expr_table_output_stream << std::endl;
    }
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

// Auto-Gen for the Param(Rel MaxDiff) Experiment

void GenParamRelDiffTableCR(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "param_rel_diff_cr" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &max_diff : kMaxDiffRel) {
    for (const auto &method : kMethodListRel) {
      expr_table_output_stream << method << ",";
      for (const auto &data_set : kDataSetList) {
        ExprConf this_conf = ExprConf(method, data_set, kBlockSizeRel, max_diff);
        expr_table_output_stream << expr_table.find(this_conf)->second.CalCompressionRatio(this_conf) << ",";
      }
      expr_table_output_stream << std::endl;
    }
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenParamRelDiffTableCT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "param_rel_diff_ct" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &max_diff : kMaxDiffRel) {
    for (const auto &method : kMethodListRel) {
      expr_table_output_stream << method << ",";
      for (const auto &data_set : kDataSetList) {
        ExprConf this_conf = ExprConf(method, data_set, kBlockSizeRel, max_diff);
        expr_table_output_stream << expr_table.find(this_conf)->second.AvgCompressionTimePerBlock() << ",";
      }
      expr_table_output_stream << std::endl;
    }
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenParamRelDiffTableDT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "param_rel_diff_dt" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &max_diff : kMaxDiffRel) {
    for (const auto &method : kMethodListRel) {
      expr_table_output_stream << method << ",";
      for (const auto &data_set : kDataSetList) {
        ExprConf this_conf = ExprConf(method, data_set, kBlockSizeRel, max_diff);
        expr_table_output_stream << expr_table.find(this_conf)->second.AvgDecompressionTimePerBlock() << ",";
      }
      expr_table_output_stream << std::endl;
    }
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

// Auto-Gen for the Ablation Experiment

void GenAblationTableCR(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "ablation_cr" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodListAblation) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetList) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSizeAblation, kMaxDiffAblation);
      expr_table_output_stream << expr_table.find(this_conf)->second.CalCompressionRatio(this_conf) << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenAblationTableCT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "ablation_ct" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodListAblation) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetList) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSizeAblation, kMaxDiffAblation);
      expr_table_output_stream << expr_table.find(this_conf)->second.AvgCompressionTimePerBlock() << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void GenAblationTableDT(ExprTable &expr_table) {
  std::ofstream expr_table_output_stream(kExportExprTablePrefix + "ablation_dt" + kExportExprTableSuffix);
  if (!expr_table_output_stream.is_open()) {
    std::cerr << "Failed to export performance data." << std::endl;
    exit(-1);
  }

  expr_table_output_stream << std::setiosflags(std::ios::fixed) << std::setprecision(6);

  for (const auto &method : kMethodListAblation) {
    expr_table_output_stream << method << ",";
    for (const auto &data_set : kDataSetList) {
      ExprConf this_conf = ExprConf(method, data_set, kBlockSizeAblation, kMaxDiffAblation);
      expr_table_output_stream << expr_table.find(this_conf)->second.AvgDecompressionTimePerBlock() << ",";
    }
    expr_table_output_stream << std::endl;
  }

  expr_table_output_stream.flush();
  expr_table_output_stream.close();
}

void PerfSerfXOR(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                 const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("SerfXOR", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSerfQt(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("SerfQt", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfDeflate(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                 const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("Deflate", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfLZ4(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
             const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("LZ4", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfFPC(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
             const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("FPC", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfZstd(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
              const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    char compression_output[block_size * 10];
    double decompression_output[block_size];

    auto compression_start_time = std::chrono::steady_clock::now();
    size_t compression_output_len = ZSTD_compress(compression_output, block_size * 10, original_data.data(),
                                                  original_data.size() * sizeof(double), ZSTD_defaultCLevel());
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    ZSTD_decompress(decompression_output, block_size * sizeof(double), compression_output, compression_output_len);
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
  table_to_insert.insert(std::make_pair(ExprConf("Zstd", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSnappy(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    std::string compression_output;
    std::string decompression_output;
    auto compression_start_time = std::chrono::steady_clock::now();
    size_t compression_output_len = snappy::Compress(reinterpret_cast<const char *>(original_data.data()),
                                                     original_data.size() * sizeof(double), &compression_output);
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
  table_to_insert.insert(std::make_pair(ExprConf("Snappy", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfElf(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
             const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("Elf", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfChimp128(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                  const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("Chimp128", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfGorilla(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                 const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("Gorilla", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfLZ77(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
              const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("LZ77", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfMachete(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                 const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("Machete", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSZ2(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
             const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    size_t compression_output_len;
    auto decompression_output = new double[block_size];

    auto compression_start_time = std::chrono::steady_clock::now();
    auto compression_output = SZ_compress_args(SZ_DOUBLE, original_data.data(), &compression_output_len,
                                               ABS, max_diff * 0.99, 0, 0, 0, 0, 0, 0, original_data.size());
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    size_t decompression_output_len = SZ_decompress_args(SZ_DOUBLE, compression_output,
                                                         compression_output_len, decompression_output, 0, 0,
                                                         0, 0, block_size);
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
  table_to_insert.insert(std::make_pair(ExprConf("SZ2", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSimPiece(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                  const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    std::vector<Point> input_points;
    for (int i = 0; i < original_data.size(); ++i) {
      input_points.emplace_back(i, original_data[i]);
    }

    char *compression_output = new char[original_data.size() * 8];
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
  table_to_insert.insert(std::make_pair(ExprConf("SimPiece", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSprintz(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
             const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    DoubleSprintzCompressor sprintz_compressor(max_diff);
    auto *compression_output = new int16_t [original_data.size() * 8];
    DoubleSprintzDecompressor sprintz_decompressor;

    auto compression_start_time = std::chrono::steady_clock::now();
    int compression_size = sprintz_compressor.compress(original_data, compression_output);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_size * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    sprintz_decompressor.decompress(compression_output);
    auto decompression_end_time = std::chrono::steady_clock::now();

    auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        compression_end_time - compression_start_time);
    auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        decompression_end_time - decompression_start_time);

    perf_record.IncreaseCompressionTime(compression_time_in_a_block);
    perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);

    delete[] compression_output;
  }

  perf_record.set_block_count(block_count);
  table_to_insert.insert(std::make_pair(ExprConf("Sprintz", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfALP(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
             const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    uint8_t compress_output_buffer[(block_size * sizeof(double)) + 8096];
    auto decompress_buffer_size = alp::AlpApiUtils::align_value<size_t, alp::config::VECTOR_SIZE>(block_size);
    double decompress_output_buffer[decompress_buffer_size];
    alp::AlpCompressor compressor;
    alp::AlpDecompressor decompressor;
    auto compression_start_time = std::chrono::steady_clock::now();
    compressor.compress(original_data.data(), original_data.size(), compress_output_buffer);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compressor.get_size() * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    decompressor.decompress(compress_output_buffer, block_size, decompress_output_buffer);
    auto decompression_end_time = std::chrono::steady_clock::now();

    auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        compression_end_time - compression_start_time);
    auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        decompression_end_time - decompression_start_time);

    perf_record.IncreaseCompressionTime(compression_time_in_a_block);
    perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
  }

  perf_record.set_block_count(block_count);
  table_to_insert.insert(std::make_pair(ExprConf("ALP", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

// Single Precision

void PerfSerfXOR_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                    const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfXORCompressor32 serf_xor_compressor(1000, max_diff);
  SerfXORDecompressor32 serf_xor_decompressor;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) serf_xor_compressor.AddValue(value);
    serf_xor_compressor.Close();
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(serf_xor_compressor.compressed_size_last_block());
    Array<uint8_t> compression_output = serf_xor_compressor.compressed_bytes_last_block();

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
  table_to_insert.insert(std::make_pair(ExprConf("SerfXOR", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSerfQt_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                   const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfQtCompressor32 serf_qt_compressor(block_size, max_diff * 0.97f);
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
  table_to_insert.insert(std::make_pair(ExprConf("SerfQt", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfDeflate_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                    const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("Deflate", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfLZ4_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("LZ4", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfLZ77_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                 const std::string &data_set, ExprTable &table_to_insert) {
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
  table_to_insert.insert(std::make_pair(ExprConf("LZ77", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSnappy_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                   const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    std::string compression_output;
    std::string decompression_output;
    auto compression_start_time = std::chrono::steady_clock::now();
    size_t compression_output_len = snappy::Compress(reinterpret_cast<const char *>(original_data.data()),
                                                     original_data.size() * sizeof(float), &compression_output);
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
  table_to_insert.insert(std::make_pair(ExprConf("Snappy", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfZstd_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                 const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    char compression_output[block_size * 10];
    float decompression_output[block_size];

    auto compression_start_time = std::chrono::steady_clock::now();
    size_t compression_output_len = ZSTD_compress(compression_output, block_size * 10, original_data.data(),
                                                  original_data.size() * sizeof(float), 3);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    ZSTD_decompress(decompression_output, block_size * sizeof(float), compression_output, compression_output_len);
    auto decompression_end_time = std::chrono::steady_clock::now();

    auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        compression_end_time - compression_start_time);
    auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        decompression_end_time - decompression_start_time);

    perf_record.IncreaseCompressionTime(compression_time_in_a_block);
    perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
  }

  perf_record.set_block_count(block_count);
  table_to_insert.insert(std::make_pair(ExprConf("Zstd", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSZ2_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    size_t compression_output_len;
    auto decompression_output = new float[block_size];

    auto compression_start_time = std::chrono::steady_clock::now();
    auto compression_output = SZ_compress_args(SZ_FLOAT, original_data.data(), &compression_output_len,
                                               ABS, max_diff * 0.97, 0, 0, 0, 0, 0, 0, original_data.size());
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    size_t decompression_output_len = SZ_decompress_args(SZ_FLOAT, compression_output,
                                                         compression_output_len, decompression_output, 0, 0,
                                                         0, 0, block_size);
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
  table_to_insert.insert(std::make_pair(ExprConf("SZ2", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfElf_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    uint8_t *compression_output_buffer;
    auto *decompression_output = new float[block_size];
    ssize_t compression_output_len_in_bytes;
    ssize_t decompression_len;

    auto compression_start_time = std::chrono::steady_clock::now();
    compression_output_len_in_bytes = elf_encode_32(original_data.data(), original_data.size(),
                                                    &compression_output_buffer, 0);
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len_in_bytes * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    decompression_len = elf_decode_32(compression_output_buffer, compression_output_len_in_bytes,
                                      decompression_output, 0);
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
  table_to_insert.insert(std::make_pair(ExprConf("Elf", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfChimp128_32(std::ifstream &data_set_input_stream_ref, float max_diff, int block_size,
                     const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<float> original_data;

  while ((original_data = ReadBlock32(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    ChimpCompressor32 chimp_compressor(128);

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) {
      chimp_compressor.addValue(value);
    }
    chimp_compressor.close();
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(chimp_compressor.get_size());

    Array<uint8_t> compression_output = chimp_compressor.get_compress_pack();

    auto decompression_start_time = std::chrono::steady_clock::now();
    ChimpDecompressor32 chimp_decompressor(compression_output, 128);
    std::vector<float> decompression_output = chimp_decompressor.decompress();
    auto decompression_end_time = std::chrono::steady_clock::now();

    auto compression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        compression_end_time - compression_start_time);
    auto decompression_time_in_a_block = std::chrono::duration_cast<std::chrono::microseconds>(
        decompression_end_time - decompression_start_time);

    perf_record.IncreaseCompressionTime(compression_time_in_a_block);
    perf_record.IncreaseDecompressionTime(decompression_time_in_a_block);
  }

  perf_record.set_block_count(block_count);
  table_to_insert.insert(std::make_pair(ExprConf("Chimp128", data_set, block_size, max_diff, true), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

// Ablation

void PerfSerfXOR_Without_Shifter(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                                 const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfXORCompressor serf_xor_compressor(1000, max_diff, 0);
  SerfXORDecompressor serf_xor_decompressor(0);

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
  table_to_insert.insert(std::make_pair(ExprConf("SerfXOR_w/o_Shifter", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSerfXOR_Without_OptAppr(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                                 const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfXORCompressorNoAppr serf_xor_compressor(1000, max_diff, kFileNameToAdjustDigit.find(data_set)->second);
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
  table_to_insert.insert(std::make_pair(ExprConf("SerfXOR_w/o_OptAppr", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSerfXOR_Without_FastSearch(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                                    const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfXORCompressorNoFastSearch serf_xor_compressor(1000, max_diff, kFileNameToAdjustDigit.find(data_set)->second);
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
  table_to_insert.insert(std::make_pair(ExprConf("SerfXOR_w/o_FastSearch", data_set, block_size, max_diff),
                                        perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

// Relational error-bound

void PerfSZ2Rel(std::ifstream &data_set_input_stream_ref, double rel_diff, int block_size,
                const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;
    size_t compression_output_len;
    auto decompression_output = new double[block_size];

    auto compression_start_time = std::chrono::steady_clock::now();
    auto compression_output = SZ_compress_args(SZ_DOUBLE, original_data.data(), &compression_output_len,
                                               REL, 0, rel_diff, 0, 0, 0, 0, 0, original_data.size());
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(compression_output_len * 8);

    auto decompression_start_time = std::chrono::steady_clock::now();
    size_t decompression_output_len = SZ_decompress_args(SZ_DOUBLE, compression_output,
                                                         compression_output_len, decompression_output, 0, 0,
                                                         0, 0, block_size);
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
  table_to_insert.insert(std::make_pair(ExprConf("SZ2_Rel", data_set, block_size, rel_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSerfXORRel(std::ifstream &data_set_input_stream_ref, double rel_diff, int block_size,
                    const std::string &data_set, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfXORCompressorRel serf_xor_compressor_rel(1000, rel_diff, kFileNameToAdjustDigit.find(data_set)->second);
  SerfXORDecompressor serf_xor_decompressor(kFileNameToAdjustDigit.find(data_set)->second);

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlock(data_set_input_stream_ref, block_size)).size() == block_size) {
    ++block_count;

    auto compression_start_time = std::chrono::steady_clock::now();
    for (const auto &value : original_data) serf_xor_compressor_rel.AddValue(value);
    serf_xor_compressor_rel.Close();
    auto compression_end_time = std::chrono::steady_clock::now();

    perf_record.AddCompressedSize(serf_xor_compressor_rel.compressed_size_last_block());
    Array<uint8_t> compression_output = serf_xor_compressor_rel.compressed_bytes_last_block();

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
  table_to_insert.insert(std::make_pair(ExprConf("SerfXOR_Rel", data_set, block_size, rel_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

// Lambda Expr

void PerfSerfXORLambda(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                       const std::string &data_set, int lambda, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfXORCompressor serf_xor_compressor(1000, max_diff, lambda);
  SerfXORDecompressor serf_xor_decompressor(lambda);

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
  table_to_insert.insert(std::make_pair(ExprConf("SerfXOR", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSerfXORLambdaRel(std::ifstream &data_set_input_stream_ref, double max_diff, int block_size,
                       const std::string &data_set, int lambda, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfXORCompressorRel serf_xor_compressor(1000, max_diff, lambda);
  SerfXORDecompressor serf_xor_decompressor(lambda);

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
  table_to_insert.insert(std::make_pair(ExprConf("SerfXOR_Rel", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

// Beta experiment
void PerfSerfXORBeta(std::ifstream &data_set_input_stream_ref, const std::string &data_set, double max_diff,
                     int block_size, int beta, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfXORCompressor serf_xor_compressor(1000, max_diff, kFileNameToAdjustDigit.find(data_set)->second);
  SerfXORDecompressor serf_xor_decompressor(kFileNameToAdjustDigit.find(data_set)->second);

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlockUsingBeta(data_set_input_stream_ref, block_size, beta)).size() == block_size) {
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
  table_to_insert.insert(std::make_pair(ExprConf("SerfXOR", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfSerfQtBeta(std::ifstream &data_set_input_stream_ref, const std::string &data_set, double max_diff,
                    int block_size, int beta, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  SerfQtCompressor serf_qt_compressor(block_size, max_diff);
  SerfQtDecompressor serf_qt_decompressor;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlockUsingBeta(data_set_input_stream_ref, block_size, beta)).size() == block_size) {
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
  table_to_insert.insert(std::make_pair(ExprConf("SerfQt", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

void PerfElfBeta(std::ifstream &data_set_input_stream_ref, const std::string &data_set, double max_diff,
                 int block_size, int beta, ExprTable &table_to_insert) {
  PerfRecord perf_record;

  int block_count = 0;
  std::vector<double> original_data;

  while ((original_data = ReadBlockUsingBeta(data_set_input_stream_ref, block_size, beta)).size() == block_size) {
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
  table_to_insert.insert(std::make_pair(ExprConf("Elf", data_set, block_size, max_diff), perf_record));
  ResetFileStream(data_set_input_stream_ref);
}

TEST(Perf, Overall) {
  ExprTable expr_table_overall;

  for (const auto &data_set : kDataSetList) {
    std::ifstream data_input_stream(kDataSetDirPrefix + data_set);
    if (!data_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    // Lossy Compression
    PerfSerfXOR(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfSerfQt(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfMachete(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfSZ2(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfSimPiece(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfSprintz(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);

    // Lossless Compression
    PerfGorilla(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfChimp128(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfDeflate(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfElf(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfFPC(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfLZ4(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfSerfXOR(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfLZ77(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfZstd(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);
    PerfSnappy(data_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, expr_table_overall);

    data_input_stream.close();
  }

  GenOverallTableCR(expr_table_overall);
  GenOverallTableCT(expr_table_overall);
  GenOverallTableDT(expr_table_overall);
}

TEST(Perf, ParamAbsMaxDiff) {
  ExprTable expr_table_abs_diff;

  for (const auto &data_set : kDataSetList) {
    std::ifstream data_input_stream(kDataSetDirPrefix + data_set);
    if (!data_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    for (const auto &max_diff : kMaxDiffList) {
      PerfSerfXOR(data_input_stream, max_diff, kBlockSizeParamAbsMaxDiff, data_set, expr_table_abs_diff);
      PerfSerfQt(data_input_stream, max_diff, kBlockSizeParamAbsMaxDiff, data_set, expr_table_abs_diff);
      PerfSimPiece(data_input_stream, max_diff, kBlockSizeParamAbsMaxDiff, data_set, expr_table_abs_diff);
      PerfSZ2(data_input_stream, max_diff, kBlockSizeParamAbsMaxDiff, data_set, expr_table_abs_diff);
      PerfMachete(data_input_stream, max_diff, kBlockSizeParamAbsMaxDiff, data_set, expr_table_abs_diff);
      PerfSprintz(data_input_stream, max_diff, kBlockSizeParamAbsMaxDiff, data_set, expr_table_abs_diff);
    }
  }

  GenParamAbsDiffTable(expr_table_abs_diff);
}

TEST(Perf, ParamBlockSize) {
  ExprTable expr_table_block_size;

  for (const auto &data_set : kDataSetList) {
    std::ifstream data_input_stream(kDataSetDirPrefix + data_set);
    if (!data_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    for (const auto & block_size : kBlockSizeList) {
      PerfSerfXOR(data_input_stream, kAbsMaxDiffParamBlockSize, block_size, data_set, expr_table_block_size);
      PerfSerfQt(data_input_stream, kAbsMaxDiffParamBlockSize, block_size, data_set, expr_table_block_size);
      PerfSimPiece(data_input_stream, kAbsMaxDiffParamBlockSize, block_size, data_set, expr_table_block_size);
      PerfSZ2(data_input_stream, kAbsMaxDiffParamBlockSize, block_size, data_set, expr_table_block_size);
      PerfMachete(data_input_stream, kAbsMaxDiffParamBlockSize, block_size, data_set, expr_table_block_size);
      PerfSprintz(data_input_stream, kAbsMaxDiffParamBlockSize, block_size, data_set, expr_table_block_size);
      if (block_size >= 600) {
        PerfALP(data_input_stream, kAbsMaxDiffParamBlockSize, block_size, data_set, expr_table_block_size);
      }
    }
  }

  GenParamBlockSizeTable(expr_table_block_size);
}

TEST(Perf, Rel) {
  ExprTable expr_table_rel;

  for (const auto &data_set : kDataSetList) {
    std::ifstream data_input_stream(kDataSetDirPrefix + data_set);
    if (!data_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    for (const auto &rel_diff : kMaxDiffRel) {
      PerfSerfXORRel(data_input_stream, rel_diff, kBlockSizeOverall, data_set, expr_table_rel);
      PerfSZ2Rel(data_input_stream, rel_diff, kBlockSizeOverall, data_set, expr_table_rel);
    }

    data_input_stream.close();
  }

  GenParamRelDiffTableCR(expr_table_rel);
  GenParamRelDiffTableCT(expr_table_rel);
  GenParamRelDiffTableDT(expr_table_rel);
}

TEST(Perf, SinglePrecision) {
  ExprTable expr_table_32;

  for (const auto &data_set : kDataSetList32) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    // Lossy
    PerfSerfXOR_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);
    PerfSerfQt_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);
    PerfSZ2_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);

    // Lossless
    PerfChimp128_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);
    PerfDeflate_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);
    PerfElf_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);
    PerfLZ4_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);
    PerfLZ77_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);
    PerfZstd_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);
    PerfSnappy_32(data_set_input_stream, kMaxDiff32, kBlockSize32, data_set, expr_table_32);

    data_set_input_stream.close();
  }

  GenSinglePrecisionTableCR(expr_table_32);
  GenSinglePrecisionTableCT(expr_table_32);
  GenSinglePrecisionTableDT(expr_table_32);
}

TEST(Perf, Serf_Ablation) {
  ExprTable expr_table_ablation;

  for (const auto &data_set : kDataSetList) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    PerfSerfXOR(data_set_input_stream, kMaxDiffAblation, kBlockSizeAblation, data_set, expr_table_ablation);
    PerfSerfXOR_Without_Shifter(data_set_input_stream,
                                kMaxDiffAblation,
                                kBlockSizeAblation,
                                data_set,
                                expr_table_ablation);
    PerfSerfXOR_Without_OptAppr(data_set_input_stream,
                                kMaxDiffAblation,
                                kBlockSizeAblation,
                                data_set,
                                expr_table_ablation);
    PerfSerfXOR_Without_FastSearch(data_set_input_stream,
                                   kMaxDiffAblation,
                                   kBlockSizeAblation,
                                   data_set,
                                   expr_table_ablation);

    data_set_input_stream.close();
  }

  GenAblationTableCR(expr_table_ablation);
  GenAblationTableCT(expr_table_ablation);
  GenAblationTableDT(expr_table_ablation);
}

TEST(Perf, Lambda) {
  std::ofstream result_output(kExportExprTablePrefix + "lambda_ct" + kExportExprTableSuffix);
  if (!result_output.is_open()) std::cout << "Failed to creat perf result file." << std::endl;

  for (const auto &factor : kLambdaFactorList) {
    result_output << factor << ",";
    for (const auto &data_set : kDataSetList) {
      std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
      if (!data_set_input_stream.is_open()) {
        std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
      }
      int lambda_for_this_data_set = kFileNameToAdjustDigit.find(data_set)->second;
      ExprTable expr_table_lambda;
      ExprConf this_conf = ExprConf("SerfXOR", data_set, kBlockSizeOverall, kMaxDiffOverall);
      int test_lambda = static_cast<int>((factor * lambda_for_this_data_set));
      PerfSerfXORLambda(data_set_input_stream, kMaxDiffOverall, kBlockSizeOverall, data_set, test_lambda,
                           expr_table_lambda);
      result_output << expr_table_lambda.find(this_conf)->second.AvgCompressionTimePerBlock() << ",";
      data_set_input_stream.close();
    }
    result_output << std::endl;
  }
}

TEST(Perf, Beta) {
  const static std::string chosen_data_set = "Motor-temp.csv";
  const static int min_beta = 1;
  const static int max_beta = 15;

  std::ofstream result_output(kExportExprTablePrefix + "beta_cr" + kExportExprTableSuffix);
  if (!result_output.is_open()) std::cout << "Failed to creat perf result file." << std::endl;

  std::ifstream data_set_input_stream(kDataSetDirPrefix + chosen_data_set);
  if (!data_set_input_stream.is_open()) {
    std::cerr << "Failed to open the file [" << chosen_data_set << "]" << std::endl;
  }

  for (int beta = min_beta; beta <= max_beta; beta++) {
    ExprTable expr_table_beta;
    PerfSerfXORBeta(data_set_input_stream, chosen_data_set, kMaxDiffOverall, kBlockSizeOverall, beta, expr_table_beta);
    PerfSerfQtBeta(data_set_input_stream, chosen_data_set, kMaxDiffOverall, kBlockSizeOverall, beta, expr_table_beta);
    PerfElfBeta(data_set_input_stream, chosen_data_set, kMaxDiffOverall, kBlockSizeOverall, beta, expr_table_beta);
    ExprConf serf_xor_conf = ExprConf("SerfXOR", chosen_data_set, kBlockSizeOverall, kMaxDiffOverall);
    ExprConf serf_qt_conf = ExprConf("SerfQt", chosen_data_set, kBlockSizeOverall, kMaxDiffOverall);
    ExprConf elf_conf = ExprConf("Elf", chosen_data_set, kBlockSizeOverall, kMaxDiffOverall);
    result_output << beta << "," << "SerfXOR,"
                  << expr_table_beta.find(serf_xor_conf)->second.CalCompressionRatio(serf_xor_conf)
                  << std::endl;
    result_output << beta << "," << "SerfQt,"
                  << expr_table_beta.find(serf_qt_conf)->second.CalCompressionRatio(serf_qt_conf)
                  << std::endl;
    result_output << beta << "," << "Elf,"
                  << expr_table_beta.find(elf_conf)->second.CalCompressionRatio(elf_conf)
                  << std::endl;
  }
}

TEST(Perf, TSBS) {
  ExprTable expr_table_tsbs;

  for (const auto &data_set : kDataSetListTSBS) {
    std::ifstream data_input_stream(kDataSetDirPrefix + data_set);
    if (!data_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    // Lossy Compression
    PerfSerfXOR(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfSerfQt(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfMachete(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfSZ2(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfSimPiece(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfSprintz(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);

    // Lossless Compression
    PerfGorilla(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfChimp128(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfDeflate(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfElf(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfFPC(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfLZ4(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfSerfXOR(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfLZ77(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfZstd(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);
    PerfSnappy(data_input_stream, kMaxDiffTSBS, kBlockSizeTSBS, data_set, expr_table_tsbs);

    data_input_stream.close();
  }

  GenTSBSTableCR(expr_table_tsbs);
  GenTSBSTableCT(expr_table_tsbs);
  GenTSBSTableDT(expr_table_tsbs);
}
