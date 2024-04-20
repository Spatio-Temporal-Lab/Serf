#ifndef LZ4_DECOMPRESSOR_H
#define LZ4_DECOMPRESSOR_H

#include <stdexcept>
#include <vector>

#include "lz4/lz4frame.h"
#include "serf/utils/Array.h"

class LZ4Decompressor {
private:
    LZ4F_decompressionContext_t decompression_context;

public:
    LZ4Decompressor();

    ~LZ4Decompressor();

    std::vector<double> decompress(const Array<char> &bs);
};

#endif //LZ4_DECOMPRESSOR_H
