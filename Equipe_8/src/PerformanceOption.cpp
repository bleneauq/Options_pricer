#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <algorithm>
#include "Option.hpp"
#include "PerformanceOption.hpp"

// Constructor
PerformanceOption::PerformanceOption(double T, PnlVect* weigths, int nbTimeSteps, int size){
    T_ = T;
    nbTimeSteps_ = nbTimeSteps;
    size_ = size;
    weigths_ = pnl_vect_copy(weigths);
}

double PerformanceOption::payoff(const PnlMat* path) {
    int nbAsset = path->n;
    long maturity = path->m;
    double finalPayoff = 0.0;

    // Loop over each path
    for (int i = 1 ; i < maturity ; i++) {
        double pathSum = 0.0;
        double num = 0.0;
        double denom = 0.0;
        for (int d = 0; d < nbAsset ; d++) {  
            num += GET(weigths_,d)*MGET(path, i, d);
            denom += GET(weigths_,d)*MGET(path, i-1, d);
        }
        pathSum = (num/denom);
        double optionPayoff = std::max(pathSum - 1, 0.0);

        // Add the payoff to the total
        finalPayoff += optionPayoff;
    }

    return 1 + finalPayoff;
}
    