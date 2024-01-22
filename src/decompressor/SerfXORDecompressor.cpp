#include "SerfXORDecompressor.h"

SerfXORDecompressor::SerfXORDecompressor() {
    in = NewInputBitStream(nullptr, 0);
}

void SerfXORDecompressor::initLeadingRepresentation() {
    int num = in.readInt(5);
    if (num == 0) {
        num = 32;
    }
    leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    leadingRepresentation = std::vector<int>(num);
    for (int i = 0; i < num; i++) {
        leadingRepresentation[i] = in.readInt(6);
    }
}

void SerfXORDecompressor::initTrailingRepresentation() {
    int num = in.readInt(5);
    if (num == 0) {
        num = 32;
    }
    trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    trailingRepresentation = std::vector<int>(num);
    for (int i = 0; i < num; i++) {
        trailingRepresentation[i] = in.readInt(6);
    }
}

void SerfXORDecompressor::setBytes(char *data, size_t data_size) {
    in = NewInputBitStream(data, data_size);
}

void SerfXORDecompressor::refresh() {
    first = true;
    endOfStream = false;
}

void SerfXORDecompressor::nextValue() {
    long value;
    int centerBits;

    if (in.readInt(1) == 1) {
        // case 1
        centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

        value = in.readLong(centerBits) << storedTrailingZeros;
        value = storedVal ^ value;
        endOfStream = value == Elf64Utils::END_SIGN;
        storedVal = value;
    } else if (in.readInt(1) == 0) {
        // case 00
        int leadAndTrail = in.readInt(leadingBitsPerValue + trailingBitsPerValue);
        int lead = leadAndTrail >> trailingBitsPerValue;
        int trail = ~(0xffffffff << trailingBitsPerValue) & leadAndTrail;
        storedLeadingZeros = leadingRepresentation[lead];
        storedTrailingZeros = trailingRepresentation[trail];
        centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

        value = in.readLong(centerBits) << storedTrailingZeros;
        value = storedVal ^ value;
        endOfStream = value == Elf64Utils::END_SIGN;
        storedVal = value;
    }
}

void SerfXORDecompressor::next() {
    if (first) {
        if (in.readBit() == 1) {
            initLeadingRepresentation();
            initTrailingRepresentation();
        }
        first = false;
        int trailingZeros = in.readInt(7);
        if (trailingZeros < 64) {
            storedVal = ((in.readLong(63 - trailingZeros) << 1) + 1) << trailingZeros;
        } else {
            storedVal = 0;
        }
        endOfStream = storedVal == Elf64Utils::END_SIGN;
    } else {
        nextValue();
    }
}

double SerfXORDecompressor::readValue() {
    next();
    return longBitsToDouble(storedVal);
}

bool SerfXORDecompressor::available() const {
    return (storedVal != Elf64Utils::END_SIGN);
}

double SerfXORDecompressor::longBitsToDouble(long bits) {
    double result;
    std::memcpy(&result, &bits, sizeof(bits));
    return result;
}
