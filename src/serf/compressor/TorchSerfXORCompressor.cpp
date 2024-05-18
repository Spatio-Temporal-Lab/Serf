#include "TorchSerfXORCompressor.h"

TorchSerfXORCompressor::TorchSerfXORCompressor(double maxDiff, long adjustD): maxDiff(maxDiff), adjustD(adjustD) {
    this->out = std::make_unique<OutputBitStream>(5 * 8);
}

Array<uint8_t> TorchSerfXORCompressor::compress(double v) {
    uint64_t thisVal;
    // note we cannot let > maxDiff, because NaN - v > maxDiff is always false
    if (std::abs(Double::longBitsToDouble(storedVal) - static_cast<double>(adjustD) - v) > maxDiff) {
        // in our implementation, we do not consider special cases and overflow case
        double adjustValue = v + static_cast<double>(adjustD);
        thisVal = Serf64Utils::findAppLong(adjustValue - maxDiff, adjustValue + maxDiff, v, storedVal, maxDiff, adjustD);
    } else {
        // let current value be the last value, making an XORed value of 0.
        thisVal = storedVal;
    }
    Array<uint8_t> result = addValue(thisVal);
    storedVal = thisVal;
    return result;
}

Array<uint8_t> TorchSerfXORCompressor::addValue(uint64_t value) {
    int thisSize = 0;
    if (numberOfValues >= BLOCK_SIZE) {
        thisSize += updateFlagAndPositionsIfNeeded();
    }
    thisSize += compressValue(value);
    compressedSizeInBits += thisSize;
    out->flush();
    uint8_t *data_ptr = out->getBuffer();
    Array<uint8_t> result(std::ceil(static_cast<double>(thisSize) / 8.0));
    for (int i = 0; i < result.length; ++i) {
        result[i] = data_ptr[i];
    }
    out->refresh();
    ++numberOfValues;
    return result;
}

int TorchSerfXORCompressor::compressValue(uint64_t value) {
    int thisSize = 0;
    uint64_t xorResult = storedVal ^ value;

    if (xorResult == 0) {
        // case 01
        if (equalWin) {
            thisSize += out->writeBit(true);
        } else {
            thisSize += out->writeInt(1, 2);
        }
        equalVote++;
    } else {
        int leadingCount = __builtin_clzll(xorResult);
        int trailingCount = __builtin_ctzll(xorResult);
        int leadingZeros = leadingRound[leadingCount];
        int trailingZeros = trailingRound[trailingCount];
        ++leadDistribution[leadingCount];
        ++trailDistribution[trailingCount];

        if (leadingZeros >= storedLeadingZeros && trailingZeros >= storedTrailingZeros &&
            (leadingZeros - storedLeadingZeros) + (trailingZeros - storedTrailingZeros) <
            1 + leadingBitsPerValue + trailingBitsPerValue) {
            // case 1
            int centerBits = 64 - storedLeadingZeros - storedTrailingZeros;
            int len;
            if (equalWin) {
                len = 2 + centerBits;
                if (len > 64) {
                    out->writeInt(1, 2);
                    out->writeLong(xorResult >> storedTrailingZeros, centerBits);
                } else {
                    out->writeLong((1ULL << centerBits) | (xorResult >> storedTrailingZeros), 2 + centerBits);
                }
            } else {
                len = 1 + centerBits;
                if (len > 64) {
                    out->writeInt(1, 1);
                    out->writeLong(xorResult >> storedTrailingZeros, centerBits);
                } else {
                    out->writeLong((1ULL << centerBits) | (xorResult >> storedTrailingZeros), 1 + centerBits);
                }
            }
            thisSize += len;
            equalVote--;
        } else {
            storedLeadingZeros = leadingZeros;
            storedTrailingZeros = trailingZeros;
            int centerBits = 64 - storedLeadingZeros - storedTrailingZeros;

            // case 00
            int len = 2 + leadingBitsPerValue + trailingBitsPerValue + centerBits;
            if (len > 64) {
                out->writeInt((leadingRepresentation[storedLeadingZeros] << trailingBitsPerValue)
                              | trailingRepresentation[storedTrailingZeros],
                              2 + leadingBitsPerValue + trailingBitsPerValue);
                out->writeLong(xorResult >> storedTrailingZeros, centerBits);
            } else {
                out->writeLong(
                        ((((uint64_t) leadingRepresentation[storedLeadingZeros] << trailingBitsPerValue) |
                          trailingRepresentation[storedTrailingZeros]) << centerBits) |
                        (xorResult >> storedTrailingZeros),
                        len
                );
            }
            thisSize += len;
        }
    }
    return thisSize;
}

int TorchSerfXORCompressor::updateFlagAndPositionsIfNeeded() {
    int len;
    equalWin = equalVote > 0;
    double thisCompressionRatio = static_cast<double>(compressedSizeInBits) / (numberOfValues * 64.0);
    if (storedCompressionRatio < thisCompressionRatio) {
        // update positions
        Array<int> leadPositions = PostOfficeSolver::initRoundAndRepresentation(leadDistribution, leadingRepresentation,
                                                                                leadingRound);
        leadingBitsPerValue = PostOfficeSolver::positionLength2Bits[leadPositions.length];
        Array<int> trailPositions = PostOfficeSolver::initRoundAndRepresentation(trailDistribution, trailingRepresentation,
                                                                                 trailingRound);
        trailingBitsPerValue = PostOfficeSolver::positionLength2Bits[trailPositions.length];
        len = static_cast<int>(out->writeInt(equalWin ? 3 : 1, 2))
              + PostOfficeSolver::writePositions(leadPositions, out.get())
              + PostOfficeSolver::writePositions(trailPositions, out.get());
    } else {
        len = static_cast<int>(out->writeInt(equalWin ? 2 : 0, 2));
    }
    equalVote = 0;
    storedCompressionRatio = thisCompressionRatio;
    numberOfValues = 0;
    for (int i = 0; i < 64; ++i) {
        leadDistribution[i] = 0;
        trailDistribution[i] = 0;
    }
    return len;
}

std::vector<uint8_t> TorchSerfXORCompressor::compress_vector(double v) {
    uint64_t thisVal;
    // note we cannot let > maxDiff, because NaN - v > maxDiff is always false
    if (std::abs(Double::longBitsToDouble(storedVal) - static_cast<double>(adjustD) - v) > maxDiff) {
        // in our implementation, we do not consider special cases and overflow case
        double adjustValue = v + static_cast<double>(adjustD);
        thisVal = Serf64Utils::findAppLong(adjustValue - maxDiff, adjustValue + maxDiff, v, storedVal, maxDiff, adjustD);
    } else {
        // let current value be the last value, making an XORed value of 0.
        thisVal = storedVal;
    }
    std::vector<uint8_t> result = addValue_vector(thisVal);
    storedVal = thisVal;
    return result;
}

std::vector<uint8_t> TorchSerfXORCompressor::addValue_vector(uint64_t value) {
    int thisSize = 0;
    if (numberOfValues >= BLOCK_SIZE) {
        thisSize += updateFlagAndPositionsIfNeeded();
    }
    thisSize += compressValue(value);
    compressedSizeInBits += thisSize;
    out->flush();
    uint8_t *data_ptr = out->getBuffer();
    int ret_len = std::ceil(static_cast<double>(thisSize) / 8.0);
    std::vector<uint8_t> result(ret_len);
    for (int i = 0; i < ret_len; ++i) {
        result[i] = data_ptr[i];
    }
    out->refresh();
    ++numberOfValues;
    return result;
}
