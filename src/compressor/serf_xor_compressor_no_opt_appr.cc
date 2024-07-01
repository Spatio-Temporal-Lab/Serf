#include "serf_xor_compressor_no_opt_appr.h"

SerfXORCompressorNoAppr::SerfXORCompressorNoAppr(int windows_size, double max_diff, long adjust_digit)
    : kWindowSize(windows_size), kMaxDiff(max_diff), kAdjustDigit(adjust_digit) {
  output_buffer_ = std::make_unique<OutputBitStream>(std::floor(((windows_size + 1) * 8 + windows_size / 8 + 1) * 1.2));
  compressed_size_this_block_ = output_buffer_->WriteInt(0, 2);
}

void SerfXORCompressorNoAppr::AddValue(double v) {
  uint64_t this_val;
  // note we cannot let > max_diff_, because kNan - v > max_diff_ is always false
  if (__builtin_expect(std::abs(Double::LongBitsToDouble(stored_val_) - kAdjustDigit - v) > kMaxDiff, false)) {
    // in our implementation, we do not consider special cases and overflow case
    double adjust_value = v + kAdjustDigit;
    this_val = SerfUtils64::FindAppLongBasic(adjust_value - kMaxDiff, adjust_value + kMaxDiff, v, stored_val_,
                                        kMaxDiff, kAdjustDigit);
  } else {
    // let current value be the last value, making an XORed value of 0.
    this_val = stored_val_;
  }

  compressed_size_this_block_ += CompressValue(this_val);
  stored_val_ = this_val;
  ++number_of_values_this_window_;
}

long SerfXORCompressorNoAppr::compressed_size_last_block() const {
  return compressed_size_last_block_;
}

Array<uint8_t> SerfXORCompressorNoAppr::compressed_bytes_last_block() {
  return compressed_bytes_last_block_;
}

void SerfXORCompressorNoAppr::Close() {
  compressed_size_this_block_ += CompressValue(Double::DoubleToLongBits(Double::kNan));
  output_buffer_->Flush();
  compressed_bytes_last_block_ = output_buffer_->GetBuffer(std::ceil((double) compressed_size_this_block_ / 8.0));
  output_buffer_->Refresh();
  compressed_size_last_block_ = compressed_size_this_block_;
  compressed_size_this_block_ = UpdateFlagAndPositionsIfNeeded();
}

int SerfXORCompressorNoAppr::CompressValue(uint64_t value) {
  int this_size = 0;
  uint64_t xor_result = stored_val_ ^ value;

  if (__builtin_expect(xor_result == 0, true)) {
    // case 01
    this_size += equal_win_ ? output_buffer_->WriteBit(true) : output_buffer_->WriteInt(1, 2);
    equal_vote_++;
  } else {
    int leading_count = __builtin_clzll(xor_result);
    int trailing_count = __builtin_ctzll(xor_result);
    int leading_zeros = leading_round_[leading_count];
    int trailing_zeros = trailing_round_[trailing_count];
    ++lead_distribution_[leading_count];
    ++trail_distribution_[trailing_count];

    if (leading_zeros >= stored_leading_zeros_ && trailing_zeros >= stored_trailing_zeros_ &&
        (leading_zeros - stored_leading_zeros_) + (trailing_zeros - stored_trailing_zeros_) <
            1 + leading_bits_per_value_ + trailing_bits_per_value_) {
      // case 1
      int center_bits = 64 - stored_leading_zeros_ - stored_trailing_zeros_;
      int len;
      if (equal_win_) {
        len = 2 + center_bits;
        if (len > 64) {
          output_buffer_->WriteInt(1, 2);
          output_buffer_->WriteLong(xor_result >> stored_trailing_zeros_, center_bits);
        } else {
          output_buffer_->WriteLong((1ULL << center_bits) | (xor_result >> stored_trailing_zeros_), 2 + center_bits);
        }
      } else {
        len = 1 + center_bits;
        if (len > 64) {
          output_buffer_->WriteInt(1, 1);
          output_buffer_->WriteLong(xor_result >> stored_trailing_zeros_, center_bits);
        } else {
          output_buffer_->WriteLong((1ULL << center_bits) | (xor_result >> stored_trailing_zeros_), 1 + center_bits);
        }
      }
      this_size += len;
      equal_vote_--;
    } else {
      stored_leading_zeros_ = leading_zeros;
      stored_trailing_zeros_ = trailing_zeros;
      int center_bits = 64 - stored_leading_zeros_ - stored_trailing_zeros_;

      // case 00
      int len = 2 + leading_bits_per_value_ + trailing_bits_per_value_ + center_bits;
      if (len > 64) {
        output_buffer_->WriteInt((leading_representation_[stored_leading_zeros_] << trailing_bits_per_value_) |
            trailing_representation_[stored_trailing_zeros_], 2 + leading_bits_per_value_ + trailing_bits_per_value_);
        output_buffer_->WriteLong(xor_result >> stored_trailing_zeros_, center_bits);
      } else {
        output_buffer_->WriteLong(((((uint64_t) leading_representation_[stored_leading_zeros_] <<
                                                                                               trailing_bits_per_value_)
            | trailing_representation_[stored_trailing_zeros_]) << center_bits) | (xor_result
            >> stored_trailing_zeros_), len);
      }
      this_size += len;
    }
  }
  return this_size;
}

int SerfXORCompressorNoAppr::UpdateFlagAndPositionsIfNeeded() {
  int len;
  equal_win_ = equal_vote_ > 0;
  if (number_of_values_this_window_ < kWindowSize) {
    // Only Check if update flag
    compressed_size_this_window_ += compressed_size_last_block_;
    len = output_buffer_->WriteInt(equal_win_ ? 2 : 0, 2);
  } else {
    double compression_ratio_this_window_ = (double) compressed_size_this_window_ / (number_of_values_this_window_ * 64);
    if (compression_ratio_last_window_ < compression_ratio_this_window_) {
      // update positions
      Array<int> lead_positions =
          PostOfficeSolver::InitRoundAndRepresentation(lead_distribution_, leading_representation_, leading_round_);
      leading_bits_per_value_ = PostOfficeSolver::kPositionLength2Bits[lead_positions.length()];
      Array<int> trail_positions = PostOfficeSolver::InitRoundAndRepresentation(trail_distribution_,
                                                                                trailing_representation_,
                                                                                trailing_round_);
      trailing_bits_per_value_ = PostOfficeSolver::kPositionLength2Bits[trail_positions.length()];
      len = output_buffer_->WriteInt(equal_win_ ? 3 : 1, 2)
          + PostOfficeSolver::WritePositions(lead_positions,
                                             output_buffer_.get())
          + PostOfficeSolver::WritePositions(trail_positions,
                                             output_buffer_.get());
    } else {
      len = output_buffer_->WriteInt(equal_win_ ? 2 : 0, 2);
    }
    compression_ratio_last_window_ = compression_ratio_this_window_;
    __builtin_memset(lead_distribution_.begin(), 0, 64 * sizeof(int));
    __builtin_memset(trail_distribution_.begin(), 0, 64 * sizeof(int));
    compressed_size_this_window_ = 0;
    number_of_values_this_window_ = 0;
  }
  equal_vote_ = 0;
  return len;
}