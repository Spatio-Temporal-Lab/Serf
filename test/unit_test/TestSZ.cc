#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "sz/sz/include/sz.h"

const static int BLOCK_SIZE = 1000;
const static std::string DATA_SET_DIR = "../../test/dataSet";

constexpr static double MAX_DIFF[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6, 1.0E-7, 1.0E-8};

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

TEST(TestSZ, CorrectnessTest) {
    std::vector<std::string> dataSetList = scanDataSet();
    for (const auto &dataSet: dataSetList) {
        std::string fileName = dataSet.substr(dataSet.find_last_of('/') + 1, dataSet.size());
        for (const auto &max_diff: MAX_DIFF) {
            std::ifstream dataSetInputStream(dataSet);
            if (!dataSetInputStream.is_open()) {
                fprintf(stderr, "[Error] Failed to open the file '%s'", dataSet.c_str());
            }

            std::vector<double> originalData;
            while ((originalData = readBlock(dataSetInputStream)).size() == BLOCK_SIZE) {
                size_t compression_output_len;
                auto decompression_output = new double[BLOCK_SIZE];
                auto compression_output = SZ_compress_args(SZ_DOUBLE, originalData.data(), &compression_output_len,
                                                           REL, max_diff, 0, 0, 0, 0, 0, 0, originalData.size());
                size_t decompression_output_len = SZ_decompress_args(SZ_DOUBLE, compression_output,
                                                                     compression_output_len, decompression_output, 0, 0,
                                                                     0, 0, BLOCK_SIZE);
                EXPECT_EQ(originalData.size(), decompression_output_len);
                for (int i = 0; i < BLOCK_SIZE; ++i) {
                    if (std::abs(originalData[i] - decompression_output[i]) > max_diff * std::abs(originalData[i])) {
                        GTEST_LOG_(INFO) << originalData[i] << " " << decompression_output[i] << " " << max_diff;
                    }
                    EXPECT_TRUE(std::abs(originalData[i] - decompression_output[i]) <= max_diff * std::abs(originalData[i]));
                }

                delete[] decompression_output;
            }

            dataSetInputStream.close();
        }
    }
}