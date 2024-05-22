#include "serf_qt_compressor.h"

SerfQtCompressor::SerfQtCompressor(int block_size, double max_diff) : kMaxDiff(max_diff * 0.999) {
  output_bit_stream_ = std::make_unique<OutputBitStream>(2 * block_size * 8);
  compressed_size_in_bits_ += output_bit_stream_->WriteInt(block_size, 16);
  compressed_size_in_bits_ += output_bit_stream_->WriteLong(Double::DoubleToLongBits(kMaxDiff), 64);
}

void SerfQtCompressor::AddValue(double v) {
  long q = static_cast<long>(std::round((v - pre_value_) / (2 * kMaxDiff)));
  double recoverValue = pre_value_ + 2 * kMaxDiff * static_cast<double>(q);
  compressed_size_in_bits_ += EliasDeltaCodec::Encode(ZigZagCodec::Encode(static_cast<int64_t>(q)) + 1,
                                                      output_bit_stream_.get());
  pre_value_ = recoverValue;
}

Array<uint8_t> SerfQtCompressor::GetBytes() {
  Array<uint8_t>
      ret = output_bit_stream_->GetBuffer(std::ceil(static_cast<double>(stored_compressed_size_in_bits_) / 8.0));
  output_bit_stream_->Refresh();
  return ret;
}

void SerfQtCompressor::Close() {
  output_bit_stream_->Flush();
  pre_value_ = 2;
  stored_compressed_size_in_bits_ = compressed_size_in_bits_;
  compressed_size_in_bits_ = 0;
}

long SerfQtCompressor::get_compressed_size_in_bits() const {
  return stored_compressed_size_in_bits_;
}
