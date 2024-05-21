#ifndef SERF_XOR_DECOMPRESSOR_H
#define SERF_XOR_DECOMPRESSOR_H

#include <cstdint>
#include <memory>
#include <vector>

#include "serf/utils/input_bit_stream.h"
#include "serf/utils/double.h"
#include "serf/utils/array.h"
#include "serf/utils/serf_utils_64.h"

class SerfXORDecompressor {
private:
    uint64_t storedVal = Double::DoubleToLongBits(2);
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();
    std::unique_ptr<InputBitStream> in = std::make_unique<InputBitStream>();
    Array<int> leadingRepresentation = {0, 8, 12, 16, 18, 20, 22, 24};
    Array<int> trailingRepresentation = {0, 22, 28, 32, 36, 40, 42, 46};
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;
    bool equalWin = false;
    long adjustD;

public:
    explicit SerfXORDecompressor(long adjustD): adjustD(adjustD) {};

    std::vector<double> decompress(const Array<uint8_t> &bs);

private:
    uint64_t readValue();
    void updateFlagAndPositionsIfNeeded();
    void updateLeadingRepresentation();
    void updateTrailingRepresentation();
};

#endif //SERF_XOR_DECOMPRESSOR_H
