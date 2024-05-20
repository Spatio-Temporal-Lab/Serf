#ifndef SERF_DOUBLE_H
#define SERF_DOUBLE_H

#include <cstdint>
#include <limits>

class Double {
public:
    static constexpr double kNan = std::numeric_limits<double>::quiet_NaN();

    static inline uint64_t doubleToLongBits(double value) {
        return *reinterpret_cast<uint64_t *>(&value);
    }

    static inline double longBitsToDouble(uint64_t bits) {
        return *reinterpret_cast<double *>(&bits);
    }
};

#endif //
