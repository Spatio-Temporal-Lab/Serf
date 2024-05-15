#include "serf/utils/InputBitStream.h"

InputBitStream::InputBitStream(uint8_t *raw_data, size_t size) {
    bool overflow = size % sizeof(uint32_t);
    len = ceil(static_cast<double>(size) / sizeof(uint32_t));
    data = new uint32_t [len];
    mem_start_addr = data;

    auto *tmp_ptr = (uint32_t *) (raw_data);
    if (overflow) {
        for (int i = 0; i < len - 1; ++i) {
            data[i] = be32toh(*(tmp_ptr + i));
        }
        int byte_index = 1;
        data[len - 1] = 0;
        for (uint64_t i = (size / 4 * 4); i < size; ++i) {
            data[len - 1] |= (raw_data[i] << (32 - byte_index * 8));
            ++byte_index;
        }
    } else {
        for (int i = 0; i < len; ++i) {
            data[i] = be32toh(*(tmp_ptr + i));
        }
    }

    buffer = ((uint64_t) data[0]) << 32;
    cursor = 1;
    bit_in_buffer = 32;
}

InputBitStream::~InputBitStream() {
    delete[] mem_start_addr;
}

uint64_t InputBitStream::peek(size_t num) {
    return buffer >> (64 - num);
}

void InputBitStream::forward(size_t num) {
    bit_in_buffer -= num;
    buffer <<= num;
    if (bit_in_buffer < 32) {
        if (cursor < len) {
            auto data_ = (uint64_t) data[cursor];
            buffer |= (data_ << (32 - bit_in_buffer));
            bit_in_buffer += 32;
            cursor++;
        } else {
            bit_in_buffer = 64;
        }
    }
}

uint64_t InputBitStream::readLong(size_t num) {
    if (num == 0) return 0;
    uint64_t result = 0;
    if (num > 32) {
        result = peek(32);
        forward(32);
        result <<= num - 32;
        num -= 32;
    }
    result |= peek(num);
    forward(num);
    return result;
}

uint32_t InputBitStream::readInt(size_t num) {
    if (num == 0) return 0;
    uint32_t result = 0;
    result |= peek(num);
    forward(num);
    return result;
}

uint32_t InputBitStream::readBit() {
    uint32_t result = peek(1);
    forward(1);
    return result;
}

void InputBitStream::setBuffer(const Array<uint8_t> &newBuffer) {
    len = std::ceil((double) newBuffer.length / sizeof(uint32_t));
    data = new uint32_t [len];
    mem_start_addr = data;
    memcpy(data, newBuffer._data.get(), newBuffer.length);
    for (int i = 0; i < len; ++i) {
        data[i] = be32toh(data[i]);
    }
    buffer = ((uint64_t) data[0]) << 32;
    cursor = 1;
    bit_in_buffer = 32;
}

void InputBitStream::setBuffer(const std::vector<uint8_t> &newBuffer) {
    bool overflow = newBuffer.size() % sizeof(uint32_t);
    len = ceil(static_cast<double>(newBuffer.size()) / sizeof(uint32_t));
    data = new uint32_t [len];
    mem_start_addr = data;

    auto *tmp_ptr = (uint32_t *) (newBuffer.data());
    if (overflow) {
        for (int i = 0; i < len - 1; ++i) {
            data[i] = be32toh(*(tmp_ptr + i));
        }
        int byte_index = 1;
        data[len - 1] = 0;
        for (uint64_t i = (newBuffer.size() / 4 * 4); i < newBuffer.size(); ++i) {
            data[len - 1] |= (newBuffer[i] << (32 - byte_index * 8));
            ++byte_index;
        }
    } else {
        for (int i = 0; i < len; ++i) {
            data[i] = be32toh(*(tmp_ptr + i));
        }
    }

    this->buffer = ((uint64_t) data[0]) << 32;
    cursor = 1;
    bit_in_buffer = 32;
}
