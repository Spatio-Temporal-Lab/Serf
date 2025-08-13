#ifndef SERF_ALL_TEST_PERF_EXPR_CONFIG_HPP_
#define SERF_ALL_TEST_PERF_EXPR_CONFIG_HPP_

#include <string>
#include <unordered_map>

// File config
const static std::string kExportExprTablePrefix = "../../test/";
const static std::string kExportExprTableSuffix = "_table.csv";
const static std::string kDataSetDirPrefix = "../../test/data_set/";
// Default data set config
const static std::string kDataSetList[] = {
    "Air-pressure.csv",
    "Basel-temp.csv",
    "Basel-wind.csv",
    "Chengdu-traj.csv",
    "City-temp.csv",
    "Dew-point-temp.csv",
    "IR-bio-temp.csv",
    "Motor-temp.csv",
    "PM10-dust.csv",
    "Smart-grid.csv",
    "Stocks-USA.csv",
    "T-drive.csv",
    "Wind-Speed.csv"
};
// Overall experiment config
const static std::string kMethodListOverall[] = {
    "LZ77", "Zstd", "Snappy", "SZ2", "Machete", "SimPiece", "Sprintz", "Deflate", "LZ4", "FPC", "Gorilla", "Chimp128",
    "Elf", "SerfQt", "SerfXOR"
};
const static double kMaxDiffOverall = 1.0E-3;
const static int kBlockSizeOverall = 50;
// Rel diff experiment config
const static std::string kMethodListRel[] = {
    "SZ2_Rel", "SerfXOR_Rel"
};
const static double kMaxDiffRel[] = {
    0.001, 0.01, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5
};
const static int kBlockSizeRel = kBlockSizeOverall;
// Param experiment (abs max_diff) config
const static std::string kMethodListParamAbsMaxDiff[] = {
    "SZ2", "Machete", "SimPiece", "SerfQt", "SerfXOR", "Sprintz"
};
const static int kBlockSizeParamAbsMaxDiff = kBlockSizeOverall;
const static double kMaxDiffList[] = {1.0E-1, 1.0E-2, 1.0E-3, 1.0E-4, 1.0E-5, 1.0E-6};
// Param experiment (block size) config
const static std::string kMethodListParamBlockSize[] = {
    "SZ2", "Machete", "SimPiece", "SerfQt", "SerfXOR", "ALP", "SZ-ADT", "Sprintz"
};
const static double kAbsMaxDiffParamBlockSize = kMaxDiffOverall;
const static int kBlockSizeList[] = {50, 100, 200, 400, 600, 800, 1000};
const static int kBlockSizeList_ADT[] = {200, 400, 600, 800, 1000};
// Single precision experiment config
const static std::string kDataSetList32[] = {
    "Basel-wind.csv",
    "Chengdu-traj.csv",
    "Dew-point-temp.csv",
    "PM10-dust.csv",
    "Smart-grid.csv",
    "Wind-Speed.csv"
};
const static std::string kMethodList32[] = {
    "LZ77", "Zstd", "Snappy", "SZ2", "Deflate", "LZ4", "Chimp128", "Elf", "SerfQt", "SerfXOR"
};
const static int kBlockSize32 = kBlockSizeOverall;
const static double kMaxDiff32 = kMaxDiffOverall;
// TSBS experiment config
const static std::string kDataSetListTSBS[] = {
    "Tsbs-iot-latitude.csv",
    "Tsbs-iot-longitude.csv"
};
const static std::string kMethodListTSBS[] = {
    "LZ77", "Zstd", "Snappy", "SZ2", "Machete", "SimPiece", "Sprintz", "Deflate", "LZ4", "FPC", "Gorilla", "Chimp128",
    "Elf", "SerfQt", "SerfXOR"
};
const static int kBlockSizeTSBS = kBlockSizeOverall;
const static double kMaxDiffTSBS = kMaxDiffOverall;
// Ablation experiment config
const static std::string kMethodListAblation[] = {
    "SerfXOR", "SerfXOR_w/o_Shifter", "SerfXOR_w/o_OptAppr", "SerfXOR_w/o_FastSearch"
};
const static int kBlockSizeAblation = kBlockSizeOverall;
const static double kMaxDiffAblation = kMaxDiffOverall;
// Lock-Up Table for SerfXOR
const static std::unordered_map<std::string, int> kFileNameToAdjustDigit{
    {"Air-pressure.csv", 0},
    {"Basel-temp.csv", 80},
    {"Basel-wind.csv", 126},
    {"Chengdu-traj.csv", 0},
    {"City-temp.csv", 355},
    {"Dew-point-temp.csv", 109},
    {"IR-bio-temp.csv", 39},
    {"Motor-temp.csv", 109},
    {"PM10-dust.csv", 256},
    {"Smart-grid.csv", 4},
    {"Stocks-USA.csv", 245},
    {"T-drive.csv", 0},
    {"Wind-Speed.csv", 8},
    {"Tsbs-iot-latitude.csv", 0},
    {"Tsbs-iot-longitude.csv", 0}
};
// Lambda config
const static double kLambdaFactorList[] = {0.2, 0.4, 0.6, 0.8, 1, 1.2, 1.4, 1.6, 1.8};

#endif //SERF_ALL_TEST_PERF_EXPR_CONFIG_HPP_
