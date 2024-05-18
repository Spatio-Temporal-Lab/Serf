#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "serf/compressor/SerfXORCompressor.h"
#include "serf/decompressor/SerfXORDecompressor.h"
#include "serf/compressor/TorchSerfXORCompressor.h"
#include "serf/decompressor/TorchSerfXORDecompressor.h"
#include "serf/compressor/SerfQtCompressor.h"
#include "serf/decompressor/SerfQtDecompressor.h"
#include "serf/compressor32/SerfXORCompressor32.h"
#include "serf/decompressor32/SerfXORDecompressor32.h"
#include "serf/compressor/NetSerfXORCompressor.h"
#include "serf/decompressor/NetSerfXORDecompressor.h"
#include "serf/compressor/NetSerfQtCompressor.h"
#include "serf/decompressor/NetSerfQtDecompressor.h"
#include "serf/compressor32/SerfQtCompressor32.h"
#include "serf/decompressor32/SerfQtDecompressor32.h"

const static int BLOCK_SIZE = 1000;
const static std::string DATA_SET_DIR = "../../test/dataSet";
const static std::unordered_map<std::string, int> FILE_TO_ADJUST_D{
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
constexpr static double MAX_DIFF[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};
constexpr static float MAX_DIFF_32[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};

/**
 * @brief Scan all data set files in DATA_SET_DIR.
 * @return A vector contains all data set file path.
 */
std::vector<std::string> scanDataSet() {
    namespace fs = std::filesystem;
    std::vector<std::string> dataSetList;
    for (const auto &entry: fs::recursive_directory_iterator(DATA_SET_DIR)) {
        if (fs::is_regular_file(entry)) {
            std::string fileNameOfEntry = entry.path().filename();
            if (fileNameOfEntry.substr(fileNameOfEntry.find('.') + 1, 3) == "csv") {
                dataSetList.push_back(entry.path().string());
            }
        }
    }
    return dataSetList;
}

/**
 * @brief Read a block of double from file input stream, whose size is equal to BLOCK_SIZE
 * @param fileInputStreamRef Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<double> readBlock(std::ifstream &fileInputStreamRef) {
    std::vector<double> returnData;
    int readDoubleCount = 0;
    double buffer;
    while (!fileInputStreamRef.eof() && readDoubleCount < BLOCK_SIZE) {
        fileInputStreamRef >> buffer;
        returnData.emplace_back(buffer);
        ++readDoubleCount;
    }
    return returnData;
}

/**
 * @brief Read a block of float from file input stream, whose size is equal to BLOCK_SIZE
 * @param fileInputStreamRef Input steam where this function reads
 * @return A vector of doubles, whose size may be less than BLOCK_SIZE
 */
std::vector<float> readBlock32(std::ifstream &fileInputStreamRef) {
    std::vector<float> returnData;
    int readDoubleCount = 0;
    float buffer;
    while (!fileInputStreamRef.eof() && readDoubleCount < BLOCK_SIZE) {
        fileInputStreamRef >> buffer;
        returnData.emplace_back(buffer);
        ++readDoubleCount;
    }
    return returnData;
}

TEST(TestSerfXOR, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
        int adjustD = FILE_TO_ADJUST_D.find(fileName)->second;
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            SerfXORCompressor xor_compressor(1000, max_diff, adjustD);
            SerfXORDecompressor xor_decompressor(adjustD);

            std::vector<double> originalData;
            while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
                for (const auto &item: originalData) {
                    xor_compressor.AddValue(item);
                }
                xor_compressor.Close();
                Array<uint8_t> result = xor_compressor.compressed_bytes();
                std::vector<double> decompressed = xor_decompressor.decompress(result);
                EXPECT_EQ(originalData.size(), decompressed.size());
                for (int i = 0; i < BLOCK_SIZE; ++i) {
                    if (std::abs(originalData[i] - decompressed[i]) > max_diff) {
                        GTEST_LOG_(INFO) << originalData[i] << " " << decompressed[i] << " " << max_diff;
                    }
                    EXPECT_TRUE(std::abs(originalData[i] - decompressed[i]) <= max_diff);
                }
            }

            dataSetInputStream.close();
        }
    }
}

TEST(TestSerfQt, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            std::vector<double> originalData;
            while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
                SerfQtCompressor qt_compressor(BLOCK_SIZE, max_diff);
                for (const auto &item: originalData) {
                    qt_compressor.AddValue(item);
                }
                qt_compressor.Close();
                Array<uint8_t> result = qt_compressor.GetBytes();
                SerfQtDecompressor qt_decompressor(result);
                std::vector<double> decompressed = qt_decompressor.Decompress();
                EXPECT_EQ(originalData.size(), decompressed.size());
                for (int i = 0; i < BLOCK_SIZE; ++i) {
                    if (std::abs(originalData[i] - decompressed[i]) > max_diff) {
                        GTEST_LOG_(INFO) << " " << originalData[i] << " " << decompressed[i] << " " << max_diff;
                    }
                    EXPECT_TRUE(std::abs(originalData[i] - decompressed[i]) <= max_diff);
                }
            }

            dataSetInputStream.close();
        }
    }
}

TEST(TestNetSerfXOR, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
        int adjustD = FILE_TO_ADJUST_D.find(fileName)->second;
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            NetSerfXORCompressor xor_compressor(max_diff, adjustD);
            NetSerfXORDecompressor xor_decompressor(adjustD);

            double originalData;
            while (!dataSetInputStream.eof()) {
                dataSetInputStream >> originalData;
                Array<uint8_t> result = xor_compressor.compress(originalData);
                double decompressed = xor_decompressor.decompress(result);
                if (std::abs(originalData - decompressed) > max_diff) {
                    GTEST_LOG_(INFO) << originalData << " " << decompressed << " " << max_diff;
                }
                EXPECT_TRUE(std::abs(originalData - decompressed) <= max_diff);
            }

            dataSetInputStream.close();
        }
    }
}

TEST(TestNetSerfQt, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            NetSerfQtCompressor qt_compressor(max_diff);
            NetSerfQtDecompressor qt_decompressor(max_diff);

            double originalData;
            while (!dataSetInputStream.eof()) {
                dataSetInputStream >> originalData;
                Array<uint8_t> result = qt_compressor.compress(originalData);
                double decompressed = qt_decompressor.decompress(result);
                if (std::abs(originalData - decompressed) > max_diff) {
                    GTEST_LOG_(INFO) << originalData << " " << decompressed << " " << max_diff;
                }
                EXPECT_TRUE(std::abs(originalData - decompressed) <= max_diff);
            }

            dataSetInputStream.close();
        }
    }
}

TEST(TestTorchSerfXOR, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
        int adjustD = FILE_TO_ADJUST_D.find(fileName)->second;
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            TorchSerfXORCompressor xor_compressor(max_diff, adjustD);
            TorchSerfXORDecompressor xor_decompressor(adjustD);

            double originalData;
            while (!dataSetInputStream.eof()) {
                dataSetInputStream >> originalData;
                Array<uint8_t> result = xor_compressor.compress(originalData);
                double decompressed = xor_decompressor.decompress(result);
                if (std::abs(originalData - decompressed) > max_diff) {
                    GTEST_LOG_(INFO) << originalData << " " << decompressed << " " << max_diff;
                }
                EXPECT_TRUE(std::abs(originalData - decompressed) <= max_diff);
            }

            dataSetInputStream.close();
        }
    }
}

TEST(TestSerfXOR32, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        for (const auto &max_diff: MAX_DIFF_32) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            SerfXORCompressor32 xor_compressor_32(BLOCK_SIZE, max_diff);
            SerfXORDecompressor32 xor_decompressor_32;

            std::vector<float> originalData;
            while ((originalData = readBlock32(dataSetInputStream)).size() == BLOCK_SIZE) {
                for (const auto &item: originalData) {
                    xor_compressor_32.addValue(item);
                }
                xor_compressor_32.close();
                Array<uint8_t> result = xor_compressor_32.getBytes();
                std::vector<float> decompressed = xor_decompressor_32.decompress(result);
                EXPECT_EQ(originalData.size(), decompressed.size());
                if (originalData.size() != decompressed.size()) {
                    GTEST_LOG_(INFO) << dataSet << " " << max_diff;
                } else {
                    for (int i = 0; i < BLOCK_SIZE; ++i) {
                        if (std::abs(originalData[i] - decompressed[i]) > max_diff) {
                            GTEST_LOG_(INFO) << originalData[i] << " " << decompressed[i] << " " << max_diff;
                        }
                        EXPECT_TRUE(std::abs(originalData[i] - decompressed[i]) <= max_diff);
                    }
                }
            }

            dataSetInputStream.close();
        }
    }
}

TEST(TestSerfQt32, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        for (const auto &max_diff: MAX_DIFF_32) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            SerfQtCompressor32 qt_compressor_32(max_diff);
            SerfQtDecompressor32 qt_decompressor_32(max_diff);

            std::vector<float> originalData;
            while ((originalData = readBlock32(dataSetInputStream)).size() == BLOCK_SIZE) {
                for (const auto &item: originalData) {
                    qt_compressor_32.addValue(item);
                }
                qt_compressor_32.close();
                Array<uint8_t> result = qt_compressor_32.getBytes();
                std::vector<float> decompressed = qt_decompressor_32.decompress(result);
                EXPECT_EQ(originalData.size(), decompressed.size());
                if (originalData.size() != decompressed.size()) {
                    GTEST_LOG_(INFO) << dataSet << " " << max_diff;
                } else {
                    for (int i = 0; i < BLOCK_SIZE; ++i) {
                        if (std::abs(originalData[i] - decompressed[i]) > max_diff) {
                            GTEST_LOG_(INFO) << originalData[i] << " " << decompressed[i] << " " << max_diff;
                        }
                        EXPECT_TRUE(std::abs(originalData[i] - decompressed[i]) <= max_diff);
                    }
                }
            }

            dataSetInputStream.close();
        }
    }
}