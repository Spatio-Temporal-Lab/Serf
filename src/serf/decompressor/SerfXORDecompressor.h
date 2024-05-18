#ifndef SERF_XOR_DECOMPRESSOR_H
#define SERF_XOR_DECOMPRESSOR_H

#include <cstdint>
#include <memory>
#include <vector>

#include "serf/utils/InputBitStream.h"
#include "serf/utils/Double.h"
#include "serf/utils/Array.h"
#include "serf/utils/Serf64Utils.h"

class SerfXORDecompressor {
private:
    const long adjust_digit_;
    uint64_t storedVal = Double::doubleToLongBits(2);
    int storedLeadingZeros = std::numeric_limits<int>::max();
    int storedTrailingZeros = std::numeric_limits<int>::max();
    std::unique_ptr<InputBitStream> in = std::make_unique<InputBitStream>();
    Array<int> leadingRepresentation = {0, 8, 12, 16, 18, 20, 22, 24};
    Array<int> trailingRepresentation = {0, 22, 28, 32, 36, 40, 42, 46};
    int leadingBitsPerValue = 3;
    int trailingBitsPerValue = 3;
    bool equalWin = false;

public:
    explicit SerfXORDecompressor(long adjust_digit): adjust_digit_(adjust_digit) {};

    std::vector<double> decompress(const Array<uint8_t> &bs);

private:
    uint64_t readValue();
    void updateFlagAndPositionsIfNeeded();
    void updateLeadingRepresentation();
    void updateTrailingRepresentation();
};

#endif //SERF_XOR_DECOMPRESSOR_H
