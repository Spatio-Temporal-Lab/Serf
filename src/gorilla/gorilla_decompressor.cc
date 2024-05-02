#include "gorilla_decompressor.h"

std::vector<double> GorillaDecompressor::decompress(const Array<uint8_t>& compress_pack) {
    input_bit_stream_->setBuffer(compress_pack);
    std::vector<double> values;
    double cur_value;
    while (!std::isnan(cur_value = nextValue())) {
        values.emplace_back(cur_value);
    }
    return values;
}

double GorillaDecompressor::nextValue() {
    if (first_) {
        first_ = false;
        pr_value_ = input_bit_stream_->readLong(64);
    } else {
        if (input_bit_stream_->readBit() == 1) {
            if (input_bit_stream_->readBit() == 1) {
                pr_lead_ = input_bit_stream_->readInt(5);
                int significant_bits = input_bit_stream_->readInt(6);
                if (significant_bits == 0) significant_bits = 64;
                pr_trail_ = 64 - significant_bits - pr_lead_;
            }
            uint64_t value = input_bit_stream_->readLong(64 - pr_lead_ - pr_trail_);
            value <<= pr_trail_;
            value = pr_value_ ^ value;
            pr_value_ = value;
        }
        // else the same
    }
    return Double::longBitsToDouble(pr_value_);
}