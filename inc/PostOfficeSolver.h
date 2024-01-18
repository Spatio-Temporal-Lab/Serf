#ifndef SERFNATIVE_POSTOFFICESOLVER_H
#define SERFNATIVE_POSTOFFICESOLVER_H

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

#include "OutputBitStream.h"

class PostOfficeResult {
private:
    std::vector<int> officePositions;
    int totalAppCost;

public:
    PostOfficeResult(std::vector<int> officePositions, int totalAppCost);

    std::vector<int> getOfficePositions() const;

    int getAppCost() const;
};

class PostOfficeSolver {
private:
    constexpr static int pow2z[] = {1, 2, 4, 8, 16, 32};

    std::vector<int> calTotalCountAndNonZerosCounts(const std::vector<int> &arr, std::vector<int> &outPreNonZerosCount,
                                                    std::vector<int> &outPostNonZerosCount);

    PostOfficeResult
    buildPostOffice(std::vector<int> &arr, int num, int nonZerosCount, std::vector<int> &preNonZerosCount,
                    std::vector<int> &postNonZerosCount);

public:
    static std::vector<int> initRoundAndRepresentation(std::vector<int> &distribution, std::vector<int> representation,
                                                       std::vector<int> round);

    static int writePositions(std::vector<int> positions, OutputBitStream out);

    constexpr static int positionLength2Bits[] = {
            0, 0, 1, 2, 2, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5,
            6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6
    };
};

#endif //SERFNATIVE_POSTOFFICESOLVER_H
