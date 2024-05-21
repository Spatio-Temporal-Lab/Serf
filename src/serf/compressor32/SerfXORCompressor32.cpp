#include "SerfXORCompressor32.h"
#include "serf/utils/serf_utils_32.h"
#include "serf/utils/post_office_solver_32.h"

SerfXORCompressor32::SerfXORCompressor32(int capacity, float maxDiff): maxDiff(maxDiff) {
    this->out = std::make_unique<OutputBitStream>(std::floor(((capacity + 1) * 4 + capacity / 4 + 1) * 1.2));
    this->compressedSizeInBits = out->WriteInt(0, 2);
}

void SerfXORCompressor32::addValue(float v) {
    uint32_t thisVal;
    // note we cannot let > maxDiff, because kNan - v > maxDiff is always false
    if (std::abs(Float::IntBitsToFloat(storedVal) - v) > maxDiff) {
        // in our implementation, we do not consider special cases and overflow case
        thisVal = SerfUtils32::FindAppInt(v - maxDiff, v + maxDiff, v,
                                          storedVal, maxDiff);
    } else {
        // let current value be the last value, making an XORed value of 0.
        thisVal = storedVal;
    }

    compressedSizeInBits += compressValue(thisVal);
    storedVal = thisVal;
    ++numberOfValues;
}

long SerfXORCompressor32::getCompressedSizeInBits() const {
    return compressedSizeInBits;
}

Array<uint8_t> SerfXORCompressor32::getBytes() {
    return outBuffer;
}

void SerfXORCompressor32::close() {
    compressedSizeInBits += compressValue(Float::FloatToIntBits(Float::kNan));
    out->Flush();
    Array<uint8_t> result = out->GetBuffer(std::ceil(compressedSizeInBits / 8.0));
    out->Refresh();
    storedCompressedSizeInBits = compressedSizeInBits;
    compressedSizeInBits = updateFlagAndPositionsIfNeeded();
}

int SerfXORCompressor32::compressValue(uint32_t value) {
    int thisSize = 0;
    uint32_t xorResult = storedVal ^ value;

    if (xorResult == 0) {
        // case 01
        if (equalWin) {
            thisSize += static_cast<int>(out->WriteBit(true));
        } else {
            thisSize += static_cast<int>(out->WriteInt(1, 2));
        }
        equalVote++;
    } else {
        int leadingCount = __builtin_clz(xorResult);
        int trailingCount = __builtin_ctz(xorResult);
        int leadingZeros = leadingRound[leadingCount];
        int trailingZeros = trailingRound[trailingCount];
        ++leadDistribution[leadingCount];
        ++trailDistribution[trailingCount];

        if (leadingZeros >= storedLeadingZeros && trailingZeros >= storedTrailingZeros &&
            (leadingZeros - storedLeadingZeros) + (trailingZeros - storedTrailingZeros) < 1 + leadingBitsPerValue + trailingBitsPerValue) {
            // case 1
            int centerBits = 32 - storedLeadingZeros - storedTrailingZeros;
            int len;
            if (equalWin) {
                len = 2 + centerBits;
                if (len > 32) {
                    out->WriteInt(1, 2);
                    out->WriteInt(xorResult >> storedTrailingZeros, centerBits);
                } else {
                    out->WriteInt((1 << centerBits) |
                                  (xorResult >> storedTrailingZeros),
                                  2 + centerBits);
                }
            } else {
                len = 1 + centerBits;
                if (len > 32) {
                    out->WriteInt(1, 1);
                    out->WriteInt(xorResult >> storedTrailingZeros, centerBits);
                } else {
                    out->WriteInt((1 << centerBits) |
                                  (xorResult >> storedTrailingZeros),
                                  1 + centerBits);
                }
            }
            thisSize += len;
            equalVote--;
        } else {
            storedLeadingZeros = leadingZeros;
            storedTrailingZeros = trailingZeros;
            int centerBits = 32 - storedLeadingZeros - storedTrailingZeros;

            // case 00
            int len = 2 + leadingBitsPerValue + trailingBitsPerValue + centerBits;
            if (len > 32) {
                out->WriteInt((leadingRepresentation[storedLeadingZeros]
                                      << trailingBitsPerValue)
                              | trailingRepresentation[storedTrailingZeros],
                              2 + leadingBitsPerValue + trailingBitsPerValue);
                out->WriteInt(xorResult >> storedTrailingZeros, centerBits);
            } else {
                out->WriteInt((((leadingRepresentation[storedLeadingZeros]
                        << trailingBitsPerValue)
                                | trailingRepresentation[storedTrailingZeros])
                        << centerBits)
                              | (xorResult >> storedTrailingZeros), len);
            }
            thisSize += len;
        }
    }
    return thisSize;
}

int SerfXORCompressor32::updateFlagAndPositionsIfNeeded() {
    int len;
    equalWin = equalVote > 0;
    double thisCompressionRatio = static_cast<double>(compressedSizeInBits) / (numberOfValues * 32.0);
    if (storedCompressionRatio < thisCompressionRatio) {
        // update positions
        Array<int> leadPositions = PostOfficeSolver32::InitRoundAndRepresentation(
                leadDistribution, leadingRepresentation, leadingRound);
        leadingBitsPerValue = PostOfficeSolver32::kPositionLength2Bits[leadPositions.length()];
        Array<int> trailPositions = PostOfficeSolver32::InitRoundAndRepresentation(
                trailDistribution, trailingRepresentation, trailingRound);
        trailingBitsPerValue = PostOfficeSolver32::kPositionLength2Bits[trailPositions.length()];
        len = static_cast<int>(out->WriteInt(equalWin ? 3 : 1, 2))
              + PostOfficeSolver32::WritePositions(leadPositions, out.get())
              + PostOfficeSolver32::WritePositions(trailPositions, out.get());
    } else {
        len = static_cast<int>(out->WriteInt(equalWin ? 2 : 0, 2));
    }
    equalVote = 0;
    storedCompressionRatio = thisCompressionRatio;
    numberOfValues = 0;
    for (int i = 0; i < 32; ++i) {
        leadDistribution[i] = 0;
        trailDistribution[i] = 0;
    }
    return len;
}
