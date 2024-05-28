#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "serf/compressor/serf_xor_compressor.h"
#include "serf/decompressor/serf_xor_decompressor.h"
#include "serf/compressor/serf_qt_compressor.h"
#include "serf/decompressor/serf_qt_decompressor.h"
#include "serf/compressor_32/serf_xor_compressor_32.h"
#include "serf/decompressor_32/serf_xor_decompressor_32.h"
#include "serf/compressor/net_serf_xor_compressor.h"
#include "serf/decompressor/net_serf_xor_decompressor.h"
#include "serf/compressor/net_serf_qt_compressor.h"
#include "serf/decompressor/net_serf_qt_decompressor.h"
#include "serf/compressor_32/serf_qt_compressor_32.h"
#include "serf/decompressor_32/serf_qt_decompressor_32.h"

const static int kBlockSize = 1000;
const static std::string kDataSetDirPrefix = "../../test/data_set/";
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
const static std::string kDataSetList32[] = {
    "City-temp.csv",
    "Dew-point-temp.csv",
    "IR-bio-temp.csv",
    "PM10-dust.csv",
    "Wind-Speed.csv"
};
const static std::unordered_map<std::string, int> kFileToAdjustD {
    {"init.csv", 0},
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
constexpr static double kMaxDiff[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};
constexpr static float kMaxDiff32[] = {1.0E-1f, 1.0E-2f, 1.0E-3f};

/**
 * @brief Read a block of double from file input stream, whose size is equal to BLOCK_SIZE
 * @param file_input_stream_ref Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<double> ReadBlock(std::ifstream &file_input_stream_ref) {
  std::vector<double> ret;
  ret.reserve(kBlockSize);
  int entry_count = 0;
  double buffer;
  while (!file_input_stream_ref.eof() && entry_count < kBlockSize) {
    file_input_stream_ref >> buffer;
    ret.emplace_back(buffer);
    ++entry_count;
  }
  return ret;
}

/**
 * @brief Read a block of float from file input stream, whose size is equal to BLOCK_SIZE
 * @param fileInputStreamRef Input steam where this function reads
 * @return A vector of floats, whose size may be less than BLOCK_SIZE
 */
std::vector<float> ReadBlock32(std::ifstream &file_input_stream_ref) {
  std::vector<float> ret;
  ret.reserve(kBlockSize);
  int entry_count = 0;
  float buffer;
  while (!file_input_stream_ref.eof() && entry_count < kBlockSize) {
    file_input_stream_ref >> buffer;
    ret.emplace_back(buffer);
    ++entry_count;
  }
  return ret;
}

void ResetFileStream(std::ifstream &data_set_input_stream_ref) {
  data_set_input_stream_ref.clear();
  data_set_input_stream_ref.seekg(0, std::ios::beg);
}

TEST(TestSerfXOR, CorrectnessTest) {
  for (const auto &data_set : kDataSetList) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    int adjust_digit = kFileToAdjustD.find(data_set)->second;
    for (const auto &max_diff : kMaxDiff) {
      SerfXORCompressor xor_compressor(kBlockSize, max_diff, adjust_digit);
      SerfXORDecompressor xor_decompressor(adjust_digit);

      std::vector<double> original_data;
      while ((original_data = ReadBlock(data_set_input_stream)).size() == kBlockSize) {
        for (const auto &datum : original_data) {
          xor_compressor.AddValue(datum);
        }
        xor_compressor.Close();
        Array<uint8_t> result = xor_compressor.compressed_bytes();
        std::vector<double> decompressed = xor_decompressor.Decompress(result);
        EXPECT_EQ(original_data.size(), decompressed.size());
        for (int i = 0; i < kBlockSize; ++i) {
          if (std::abs(original_data[i] - decompressed[i]) > max_diff) {
            GTEST_LOG_(INFO) << original_data[i] << " " << decompressed[i] << " " << max_diff;
          }
          EXPECT_TRUE(std::abs(original_data[i] - decompressed[i]) <= max_diff);
        }
      }

      ResetFileStream(data_set_input_stream);
    }

    data_set_input_stream.close();
  }
}

TEST(TestSerfQt, CorrectnessTest) {
  for (const auto &data_set : kDataSetList) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    for (const auto &max_diff : kMaxDiff) {
      SerfQtCompressor qt_compressor(kBlockSize, max_diff);
      SerfQtDecompressor qt_decompressor;

      std::vector<double> original_data;
      while ((original_data = ReadBlock(data_set_input_stream)).size() == kBlockSize) {
        for (const auto &datum : original_data) {
          qt_compressor.AddValue(datum);
        }
        qt_compressor.Close();
        Array<uint8_t> result = qt_compressor.compressed_bytes();
        std::vector<double> decompressed = qt_decompressor.Decompress(result);
        EXPECT_EQ(original_data.size(), decompressed.size());
        for (int i = 0; i < kBlockSize; ++i) {
          if (std::abs(original_data[i] - decompressed[i]) > max_diff) {
            GTEST_LOG_(INFO) << original_data[i] << " " << decompressed[i] << " " << max_diff;
          }
          EXPECT_TRUE(std::abs(original_data[i] - decompressed[i]) <= max_diff);
        }
      }

      ResetFileStream(data_set_input_stream);
    }

    data_set_input_stream.close();
  }
}

TEST(TestSerfQt, GetQtResult) {
  std::string data_set = kDataSetList[5];
  double max_diff = 1.0E-6;

  std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
  if (!data_set_input_stream.is_open()) {
    std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
  }

  SerfQtCompressor qt_compressor(100000, max_diff);
  SerfQtDecompressor qt_decompressor;

  std::vector<double> original_data;
  while ((original_data = ReadBlock(data_set_input_stream)).size() == kBlockSize) {
    for (const auto &datum : original_data) {
      qt_compressor.AddValue(datum);
    }
  }
  qt_compressor.Close();
  qt_compressor.compressed_bytes();

  data_set_input_stream.close();
}

TEST(TestNetSerfXOR, CorrectnessTest) {
  for (const auto &data_set : kDataSetList) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    int adjust_digit = kFileToAdjustD.find(data_set)->second;
    for (const auto &max_diff : kMaxDiff) {
      NetSerfXORCompressor net_serf_xor_compressor(kBlockSize, max_diff, adjust_digit);
      NetSerfXORDecompressor net_serf_xor_decompressor(kBlockSize, adjust_digit);

      double originalData;
      while (!data_set_input_stream.eof()) {
        data_set_input_stream >> originalData;
        Array<uint8_t> result = net_serf_xor_compressor.Compress(originalData);
        double decompressed = net_serf_xor_decompressor.Decompress(result);
        if (std::abs(originalData - decompressed) > max_diff) {
          GTEST_LOG_(INFO) << originalData << " " << decompressed << " " << max_diff;
        }
        EXPECT_TRUE(std::abs(originalData - decompressed) <= max_diff);
      }

      ResetFileStream(data_set_input_stream);
    }

    data_set_input_stream.close();
  }
}

TEST(TestNetSerfQt, CorrectnessTest) {
  for (const auto &data_set : kDataSetList) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    for (const auto &max_diff : kMaxDiff) {
      NetSerfQtCompressor net_serf_qt_compressor(max_diff);
      NetSerfQtDecompressor net_serf_qt_decompressor(max_diff);

      double originalData;
      while (!data_set_input_stream.eof()) {
        data_set_input_stream >> originalData;
        Array<uint8_t> result = net_serf_qt_compressor.Compress(originalData);
        double decompressed = net_serf_qt_decompressor.Decompress(result);
        if (std::abs(originalData - decompressed) > max_diff) {
          GTEST_LOG_(INFO) << originalData << " " << decompressed << " " << max_diff;
        }
        EXPECT_TRUE(std::abs(originalData - decompressed) <= max_diff);
      }

      ResetFileStream(data_set_input_stream);
    }

    data_set_input_stream.close();
  }
}

TEST(TestSerfXOR32, CorrectnessTest) {
  for (const auto &data_set : kDataSetList) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    for (const auto &max_diff : kMaxDiff32) {
      SerfXORCompressor32 xor_compressor_32(kBlockSize, max_diff);
      SerfXORDecompressor32 xor_decompressor_32;

      std::vector<float> original_data;
      while ((original_data = ReadBlock32(data_set_input_stream)).size() == kBlockSize) {
        for (const auto &datum : original_data) {
          xor_compressor_32.AddValue(datum);
        }
        xor_compressor_32.Close();
        Array<uint8_t> result = xor_compressor_32.compressed_bytes();
        std::vector<float> decompressed = xor_decompressor_32.Decompress(result);
        EXPECT_EQ(original_data.size(), decompressed.size());
        for (int i = 0; i < kBlockSize; ++i) {
          if (std::abs(original_data[i] - decompressed[i]) > max_diff) {
            GTEST_LOG_(INFO) << original_data[i] << " " << decompressed[i] << " " << max_diff;
          }
          EXPECT_TRUE(std::abs(original_data[i] - decompressed[i]) <= max_diff);
        }
      }

      ResetFileStream(data_set_input_stream);
    }

    data_set_input_stream.close();
  }
}

TEST(TestSerfQt32, CorrectnessTest) {
  for (const auto &data_set : kDataSetList32) {
    std::ifstream data_set_input_stream(kDataSetDirPrefix + data_set);
    if (!data_set_input_stream.is_open()) {
      std::cerr << "Failed to open the file [" << data_set << "]" << std::endl;
    }

    for (const auto &max_diff : kMaxDiff32) {
      SerfQtCompressor32 qt_compressor_32(kBlockSize, max_diff);
      SerfQtDecompressor32 qt_decompressor_32;

      std::vector<float> original_data;
      while ((original_data = ReadBlock32(data_set_input_stream)).size() == kBlockSize) {
        for (const auto &datum : original_data) {
          qt_compressor_32.AddValue(datum);
        }
        qt_compressor_32.Close();
        Array<uint8_t> result = qt_compressor_32.compressed_bytes();
        std::vector<float> decompressed = qt_decompressor_32.Decompress(result);
        EXPECT_EQ(original_data.size(), decompressed.size());
        for (int i = 0; i < kBlockSize; ++i) {
          if (std::abs(original_data[i] - decompressed[i]) > max_diff) {
            GTEST_LOG_(INFO) << original_data[i] << " " << decompressed[i] << " " << max_diff;
          }
          EXPECT_TRUE(std::abs(original_data[i] - decompressed[i]) <= max_diff);
        }
      }

      ResetFileStream(data_set_input_stream);
    }

    data_set_input_stream.close();
  }
}