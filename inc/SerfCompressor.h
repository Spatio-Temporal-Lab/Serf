#ifndef SERFNATIVE_SERFCOMPRESSOR_H
#define SERFNATIVE_SERFCOMPRESSOR_H

#include "SerfXORCompressor.h"

#include <iostream>
#include <cstring>

typedef unsigned long b64;

class SerfCompressor {
private:
    SerfXORCompressor xor_compressor;
    int compressedSizeInBits = 0;
    int numberOfValues = 0;
    double storedCompressionRatio = 0;
    int fAlpha;
    double maxDiff;
    double NaN = longBitsToDouble(0x7ff8000000000000L);
    double storedErasedDoubleValue = NaN;
    b64 storedErasedLongValue = doubleToLongBits(NaN);

public:
    explicit SerfCompressor(double errorBound);

    b64 doubleToLongBits(double value);

    double longBitsToDouble(b64 bits);

    void addValue(double v);

    long getCompressedSizeInBits() const;

    std::vector<char> getBytes();

    void close();

    void refresh();
};


#endif //SERFNATIVE_SERFCOMPRESSOR_H
