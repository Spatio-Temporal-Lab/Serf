#ifndef SERF_QT_COMPRESSOR_32_H
#define SERF_QT_COMPRESSOR_32_H

#include <cstdint>
#include <memory>
#include <cmath>

#include "serf/utils/output_bit_stream.h"
#include "serf/utils/float.h"
#include "serf/utils/elias_delta_codec.h"
#include "serf/utils/zig_zag_codec.h"

class SerfQtCompressor32 {
private:
    const int BLOCK_SIZE = 1000;
    const float maxDiff;
    std::unique_ptr<OutputBitStream> out = std::make_unique<OutputBitStream>(2 * BLOCK_SIZE * 32);
    float preValue = 2;
    long compressedBits = 0;
    long storedCompressedBits;

public:
    explicit SerfQtCompressor32(float maxDiff);

    void addValue(float v);

    Array<uint8_t> getBytes();

    void close();

    long getCompressedSizeInBits();
};

#endif //SERF_QT_COMPRESSOR_32_H