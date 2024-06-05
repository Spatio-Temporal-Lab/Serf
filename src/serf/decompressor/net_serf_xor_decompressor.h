#ifndef NET_SERF_XOR_DECOMPRESSOR_H
#define NET_SERF_XOR_DECOMPRESSOR_H

#include <cstdint>
#include <memory>
#include <limits>

#include "serf/utils/double.h"
#include "serf/utils/input_bit_stream.h"
#include "serf/utils/post_office_solver.h"

class NetSerfXORDecompressor {
 public:
  explicit NetSerfXORDecompressor(int capacity);
  double Decompress(Array<uint8_t> &bs);

 private:
  const int kBlockSize;

  uint64_t stored_val_ = Double::DoubleToLongBits(2);
  int stored_leading_zeros_ = std::numeric_limits<int>::max();
  int stored_trailing_zeros_ = std::numeric_limits<int>::max();
  std::unique_ptr<InputBitStream> input_bit_stream_ = std::make_unique<InputBitStream>();
  Array<int> leading_representation_ = {0, 8, 12, 16, 18, 20, 22, 24};
  Array<int> trailing_representation_ = {0, 22, 28, 32, 36, 40, 42, 46};
  int leading_bits_per_value_ = 3;
  int trailing_bits_per_value_ = 3;
  int number_of_values_ = 0;
  bool equal_win_ = false;

  uint64_t ReadValue();
  void NextValue();
  void UpdateFlagAndPositionsIfNeeded();
  void UpdateLeadingRepresentation();
  void UpdateTrailingRepresentation();
};

#endif //NET_SERF_XOR_DECOMPRESSOR_H