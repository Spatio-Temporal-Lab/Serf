#include "SerfXORDecompressor.h"
#include "serf/utils/PostOfficeSolver.h"

std::vector<double> SerfXORDecompressor::decompress(const Array<uint8_t> &bs) {
    in->setBuffer(bs);
    updateFlagAndPositionsIfNeeded();
    std::vector<double> values;
    uint64_t value;
    while ((value = readValue()) != Double::doubleToLongBits(Double::NaN)) {
        values.emplace_back(Double::longBitsToDouble(value));
        storedVal = value;
    }
    return values;
}

uint64_t SerfXORDecompressor::readValue() {
    uint64_t value = storedVal;
    int centerBits;
    if (equal_win_) {
        if (in->readInt(1) == 0) {
            if (in->readInt(1) != 1) {
                // case 00
                int lead_and_trail = in->readInt(leadingBitsPerValue + trailingBitsPerValue);
                int lead = lead_and_trail >> trailingBitsPerValue;
                int trail = ~(0xffffffff << trailingBitsPerValue) & lead_and_trail;
                storedLeadingZeros = leadingRepresentation[lead];
                storedTrailingZeros = trailingRepresentation[trail];
            }
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;
            value = in->readLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
        }
    } else {
        if (in->readInt(1) == 1) {
            // case 1
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;
            value = in->readLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
        } else if (in->readInt(1) == 0) {
            // case 00
            int leadAndTrail = static_cast<int>(in->readInt(leadingBitsPerValue + trailingBitsPerValue));
            int lead = leadAndTrail >> trailingBitsPerValue;
            int trail = ~(0xffffffff << trailingBitsPerValue) & leadAndTrail;
            storedLeadingZeros = leadingRepresentation[lead];
            storedTrailingZeros = trailingRepresentation[trail];
            centerBits = 64 - storedLeadingZeros - storedTrailingZeros;
            value = in->readLong(centerBits) << storedTrailingZeros;
            value = storedVal ^ value;
        }
    }

    return value;
}

void SerfXORDecompressor::updateFlagAndPositionsIfNeeded() {
    equal_win_ = in->readBit() == 1;
    if (in->readBit() == 1) {
        updateLeadingRepresentation();
        updateTrailingRepresentation();
    }
}

void SerfXORDecompressor::updateLeadingRepresentation() {
    int num = static_cast<int>(in->readInt(5));
    if (num == 0) {
        num = 32;
    }
    leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    leadingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        leadingRepresentation[i] = static_cast<int>(in->readInt(6));
    }
}

void SerfXORDecompressor::updateTrailingRepresentation() {
    int num = static_cast<int>(in->readInt(5));
    if (num == 0) {
        num = 32;
    }
    trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[num];
    trailingRepresentation = Array<int>(num);
    for (int i = 0; i < num; i++) {
        trailingRepresentation[i] = static_cast<int>(in->readInt(6));
    }
}
