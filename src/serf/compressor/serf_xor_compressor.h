#ifndef SERF_XOR_COMPRESSOR_H
#define SERF_XOR_COMPRESSOR_H

#include <cstdint>
#include <cmath>

#include "serf/utils/double.h"
#include "serf/utils/output_bit_stream.h"
#include "serf/utils/serf_utils_64.h"
#include "serf/utils/post_office_solver.h"
#include "serf/utils/array.h"

class SerfXORCompressor {
 public:
  SerfXORCompressor(int capacity, double max_diff);

  void AddValue(double v);

  long compressed_size_in_bits() const;

  Array<uint8_t> compressed_bytes();

  void Close();

 private:
  const double kMaxDiff;
  uint64_t stored_val_ = Double::DoubleToLongBits(2);

  std::unique_ptr<OutputBitStream> output_buffer_;
  Array<uint8_t> compressed_bytes_ = Array<uint8_t>();

  long compressed_size_in_bits_;
  long stored_compressed_size_in_bits_ = 0;
  int number_of_values_ = 0;
  double stored_compression_ratio_ = 0;

  int equal_vote_ = 0;
  bool equal_win_ = false;

  Array<int> leading_representation_ = {
      0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 2, 2, 2, 2,
      3, 3, 4, 4, 5, 5, 6, 6,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7
  };
  Array<int> leading_round_ = {
      0, 0, 0, 0, 0, 0, 0, 0,
      8, 8, 8, 8, 12, 12, 12, 12,
      16, 16, 18, 18, 20, 20, 22, 22,
      24, 24, 24, 24, 24, 24, 24, 24,
      24, 24, 24, 24, 24, 24, 24, 24,
      24, 24, 24, 24, 24, 24, 24, 24,
      24, 24, 24, 24, 24, 24, 24, 24,
      24, 24, 24, 24, 24, 24, 24, 24
  };
  Array<int> trailing_representation_ = {
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 1, 1,
      1, 1, 1, 1, 2, 2, 2, 2,
      3, 3, 3, 3, 4, 4, 4, 4,
      5, 5, 6, 6, 6, 6, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
  };
  Array<int> trailing_round_ = {
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 22, 22,
      22, 22, 22, 22, 28, 28, 28, 28,
      32, 32, 32, 32, 36, 36, 36, 36,
      40, 40, 42, 42, 42, 42, 46, 46,
      46, 46, 46, 46, 46, 46, 46, 46,
      46, 46, 46, 46, 46, 46, 46, 46,
  };

  int leading_bits_per_value_ = 3;
  int trailing_bits_per_value_ = 3;
  Array<int> lead_distribution_ = Array<int>(64);
  Array<int> trail_distribution_ = Array<int>(64);
  int stored_leading_zeros_ = std::numeric_limits<int>::max();
  int stored_trailing_zeros_ = std::numeric_limits<int>::max();

  int CompressValue(uint64_t value);
  int UpdateFlagAndPositionsIfNeeded();
};

#endif  // SERF_XOR_COMPRESSOR_H