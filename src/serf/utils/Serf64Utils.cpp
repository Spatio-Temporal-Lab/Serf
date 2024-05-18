#include "Serf64Utils.h"

uint64_t Serf64Utils::findAppLong(double min, double max, double v, uint64_t lastLong, double maxDiff, long adjust_digit) {
    if (min >= 0) {
        // both positive
        return findAppLong(min, max, 0, v, lastLong, maxDiff, adjust_digit);
    } else if (max <= 0) {
        // both negative
        return findAppLong(-max, -min, 0x8000000000000000ULL, v, lastLong, maxDiff, adjust_digit);
    } else if (lastLong >> 63 == 0) {
        // consider positive part only, to make more leading zeros
        return findAppLong(0, max, 0, v, lastLong, maxDiff, adjust_digit);
    } else {
        // consider negative part only, to make more leading zeros
        return findAppLong(0, -min, 0x8000000000000000ULL, v, lastLong, maxDiff, adjust_digit);
    }
}

uint64_t Serf64Utils::findAppLong(double minDouble, double maxDouble, uint64_t sign, double original, uint64_t lastLong,
                                  double maxDiff, long adjust_digit) {
    uint64_t min = Double::doubleToLongBits(minDouble) & 0x7fffffffffffffffULL; // may be negative zero
    uint64_t max = Double::doubleToLongBits(maxDouble);
    uint64_t frontMask = 0xffffffffffffffffULL;
    uint64_t resultLong;
    double diff;
    uint64_t append;
    for (int i = 1; i <= 64; ++i) {
        uint64_t mask = frontMask << (64 - i);
        append = (lastLong & ~mask) | (min & mask);

        if (min <= append && append <= max) {
            resultLong = append ^ sign;
            diff = Double::longBitsToDouble(resultLong) - adjust_digit - original;
            if (diff >= -maxDiff && diff <= maxDiff) {
                return resultLong;
            }
        }

        append = (append + bw[64 - i]) & 0x7fffffffffffffffL; // may be overflow
        if (append <= max) {    // append must be greater than min
            resultLong = append ^ sign;
            diff = Double::longBitsToDouble(resultLong) - adjust_digit - original;
            if (diff >= -maxDiff && diff <= maxDiff) {
                return resultLong;
            }
        }
    }

    return Double::doubleToLongBits(original + adjust_digit);    // we do not find a satisfied value, so we return the original value
}
