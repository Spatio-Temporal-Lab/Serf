#ifndef SERF_QT_COMPRESSOR_H
#define SERF_QT_COMPRESSOR_H

#include <cstdint>
#include <cmath>
#include <memory>
#include <fstream>

#include "serf/utils/output_bit_stream.h"
#include "serf/utils/array.h"
#include "serf/utils/double.h"
#include "serf/utils/elias_delta_codec.h"
#include "serf/utils/zig_zag_codec.h"

/*
 * +------------+-----------------+---------------+
 * |16bits - len|64bits - max_diff|Encoded Content|
 * +------------+-----------------+---------------+
 */

class SerfQtCompressor {
 public:
  SerfQtCompressor(int block_size, double max_diff);

  void AddValue(double v);

  Array<uint8_t> compressed_bytes();

  void Close();

  long get_compressed_size_in_bits() const;

 private:
  std::ofstream qt_result_output_;
  const double kMaxDiff;
  const int kBlockSize;
  bool first_ = true;
  std::unique_ptr<OutputBitStream> output_bit_stream_;
  Array<uint8_t> compressed_bytes_;
  double pre_value_ = 2;
  long compressed_size_in_bits_ = 0;
  long stored_compressed_size_in_bits_ = 0;
};

#endif  // SERF_QT_COMPRESSOR_H
