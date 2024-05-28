#include "serf/compressor/serf_qt_compressor.h"

SerfQtCompressor::SerfQtCompressor(int block_size, double max_diff) :kBlockSize(block_size), kMaxDiff(max_diff * 0.999) {
  output_bit_stream_ = std::make_unique<OutputBitStream>(2 * block_size * 8);
  qt_result_output_.open("qt_result.txt");
}

void SerfQtCompressor::AddValue(double v) {
  if (first_) {
    first_ = false;
    compressed_size_in_bits_ += output_bit_stream_->WriteInt(kBlockSize, 16);
    compressed_size_in_bits_ += output_bit_stream_->WriteLong(Double::DoubleToLongBits(kMaxDiff), 64);
  }
  long q = static_cast<long>(std::round((v - pre_value_) / (2 * kMaxDiff)));
  qt_result_output_ << ZigZagCodec::Encode(static_cast<int64_t>(q)) + 1 << std::endl;
  double recoverValue = pre_value_ + 2 * kMaxDiff * static_cast<double>(q);
  compressed_size_in_bits_ += EliasDeltaCodec::Encode(ZigZagCodec::Encode(static_cast<int64_t>(q)) + 1,
                                                      output_bit_stream_.get());
  pre_value_ = recoverValue;
}

Array<uint8_t> SerfQtCompressor::compressed_bytes() {
  qt_result_output_.close();
  return compressed_bytes_;
}

void SerfQtCompressor::Close() {
  qt_result_output_.flush();
  output_bit_stream_->Flush();
  compressed_bytes_ = output_bit_stream_->GetBuffer(std::ceil(compressed_size_in_bits_ / 8.0));
  output_bit_stream_->Refresh();
  first_ = true;
  pre_value_ = 2;
  stored_compressed_size_in_bits_ = compressed_size_in_bits_;
  compressed_size_in_bits_ = 0;
}

long SerfQtCompressor::get_compressed_size_in_bits() const {
  return stored_compressed_size_in_bits_;
}
