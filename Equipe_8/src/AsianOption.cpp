
#include "Option.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <algorithm>
#include "AsianOption.hpp"

// Constructor inherited from Option.hpp
AsianOption::AsianOption(double T, PnlVect* weigths, double strike, int nbTimeSteps, int size){
    T_ = T;
    nbTimeSteps_ = nbTimeSteps;
    strike_ = strike;
    size_ = size;
    weigths_ = pnl_vect_copy(weigths);
}

double AsianOption::payoff(const PnlMat* path) {
    int nbAsset = path->n;
    long maturity = path->m;
    double finalPayoff = 0.0;

    // Loop over each path
    for (int d = 0; d < nbAsset; d++) {
        double pathSum = 0.0;

        // Calculate the sum of asset prices along the path
        for (int j = 0; j < maturity; j++) {
            pathSum += MGET(path, j, d);
        }

        // Calculate the Asian option payoff for this path
        double averagePrice = pathSum/ maturity;

        // Add the payoff to the total  
        finalPayoff += GET(weigths_, d)*averagePrice;
    }

    return std::max(finalPayoff - strike_, 0.0);
}
