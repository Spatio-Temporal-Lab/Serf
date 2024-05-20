#ifndef TORCH_SERF_XOR_COMPRESSOR_H
#define TORCH_SERF_XOR_COMPRESSOR_H

#include <cstdint>
#include <memory>
#include <cmath>
#include <vector>

#include "serf/utils/OutputBitStream.h"
#include "serf/utils/Double.h"
#include "serf/utils/Array.h"
#include "serf/utils/Serf64Utils.h"
#include "serf/utils/PostOfficeSolver.h"

class TorchSerfXORCompressor {
private:
    const int BLOCK_SIZE = 1000;
    const double maxDiff;
    const long adjustD;

    uint64_t storedVal = Double::DoubleToLongBits(2);
    std::unique_ptr<OutputBitStream> out;

    int numberOfValues = 0;
    int compressedSizeInBits = 0;
    double storedCompressionRatio = 0;

    int equalVote = 0;
    bool equalWin = false;

    Array<int> leadingRepresentation = {
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 4, 4, 5, 5, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7
    };
    Array<int> leadingRound = {
            0, 0, 0, 0, 0, 0, 0, 0,
            8, 8, 8, 8, 12, 12, 12, 12,
            16, 16, 18, 18, 20, 20, 22, 22,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24,
            24, 24, 24, 24, 24, 24, 24, 24
    };
    Array<int> trailingRepresentation = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 1,
            1, 1, 1, 1, 2, 2, 2, 2,
            3, 3, 3, 3, 4, 4, 4, 4,
            5, 5, 6, 6, 6, 6, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7,
    };
    Array<int> trailingRound = {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 22, 22,
            22, 22, 22, 22, 28, 28, 28, 28,
            32, 32, 32, 32, 36, 36, 36, 36,
            40, 40, 42, 42, 42, 42, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46,
            46, 46, 46, 46, 46, 46, 46, 46,
    };

    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;
    Array<int> leadDistribution = Array<int>(64);
    Array<int> trailDistribution = Array<int>(64);
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();

public:
    TorchSerfXORCompressor(double maxDiff, long adjustD);

    Array<uint8_t> compress(double v);

    std::vector<uint8_t> compress_vector(double v);

private:
    Array<uint8_t> addValue(uint64_t value);

    std::vector<uint8_t> addValue_vector(uint64_t value);

    int compressValue(uint64_t value);

    int updateFlagAndPositionsIfNeeded();
};

#endif //TORCH_SERF_XOR_COMPRESSOR_H