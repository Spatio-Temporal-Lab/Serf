#ifndef DEFLATE_DECOMPRESSOR_H
#define DEFLATE_DECOMPRESSOR_H

#include <stdexcept>
#include <vector>
#include <cmath>

#include "deflate/deflate.h"
#include "serf/utils/Array.h"

class DeflateDecompressor {
private:
    int ret;
    z_stream strm;

public:
    DeflateDecompressor();

    ~DeflateDecompressor();

    std::vector<double> decompress(const Array<unsigned char> &bs);
};

#endif //DEFLATE_DECOMPRESSOR_H