#ifndef NET_SERF_QT_COMPRESSOR_H
#define NET_SERF_QT_COMPRESSOR_H

#include <memory>
#include <cstdint>
#include <cmath>

#include "serf/utils/Array.h"
#include "serf/utils/OutputBitStream.h"
#include "serf/utils/Double.h"
#include "serf/utils/EliasDeltaCodec.h"
#include "serf/utils/ZigZagCodec.h"

class NetSerfQtCompressor {
private:
    double preValue = 2;
    const double maxDiff;
    std::unique_ptr<OutputBitStream> out = std::make_unique<OutputBitStream>(5 * 8);

public:
    explicit NetSerfQtCompressor(double errorBound);

    Array<uint8_t> compress(double v);
};

#endif //NET_SERF_QT_COMPRESSOR_H