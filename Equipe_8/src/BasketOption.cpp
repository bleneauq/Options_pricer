#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <algorithm>
#include "Option.hpp"
#include "BasketOption.hpp"
#include <iostream>
// Constructor
BasketOption::BasketOption(double T, PnlVect* weigths, double strike, int nbTimeSteps, int size){
    T_ = T;
    nbTimeSteps_ = nbTimeSteps;
    size_ = size;
    strike_ = strike;
    weigths_ = pnl_vect_copy(weigths);
}

double BasketOption::payoff(const PnlMat* path) {
    int nbAsset = path->n;
    long maturity = path->m-1;
    double finalPayoff = 0.0;
    double pathSum = 0.0;
    // Loop over each path
    for (int d = 0; d < nbAsset; d++) {
        pathSum = GET(weigths_,d)*MGET(path, maturity, d);
        finalPayoff += pathSum;
    }
    return std::max(finalPayoff - strike_, 0.0);;
}
