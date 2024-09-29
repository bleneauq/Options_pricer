#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_random.h"
#include <math.h>
#include <cmath>
#include <iostream>
#include "ProfitAndLoss.hpp"

void
ProfitAndLoss::computePnl(PnlMat* path, double& pnl, BlackScholesModel model, Option* option, nlohmann::json j)
{

    // Set the MonteCarlo Approximator
    MonteCarlo monteCarlo = MonteCarlo(
      &model,
      option,
      pnl_rng_create(PNL_RNG_MERSENNE),
      j);
    pnl_rng_sseed(monteCarlo.rng_, time(NULL));

    int H = j.at("hedging dates number").get<int>();
    int N = j.at("timestep number").get<int>();
    double tau = option->T_ / H;
    double dt = option->T_/ N;
    double newPrice, std_dev;
    double portfolioValue = 0.0;

    PnlVect* delta = pnl_vect_create(option->size_);
    PnlVect* newDelta = pnl_vect_create(option->size_);
    PnlVect* tempDelta = pnl_vect_create(option->size_);
    PnlVect* assetPrice = pnl_vect_create(option->size_);
    PnlVect* deltaStdDev = pnl_vect_create(option->size_);

    int sizePast = 2;
    PnlMat* past = pnl_mat_create_from_zero(sizePast, option->size_);
    PnlVect* row = pnl_vect_create(option->size_);
    pnl_mat_get_row(row, path, 0);
    pnl_mat_set_row(past, row, 0);

    monteCarlo.price(newPrice, std_dev);
    monteCarlo.delta(delta, deltaStdDev);
    pnl_mat_get_row(assetPrice, path, 0);

    // Display past
    
    // Print the Price
    portfolioValue = newPrice - pnl_vect_scalar_prod(delta, assetPrice);
    for (double t = tau; t <= option->T_; t += tau) {
        pnl_mat_get_row(row, path, std::round(t * (H/option->T_)));
        pnl_mat_get_row(assetPrice, path, std::round(t * (H/option->T_)));
        pnl_mat_set_row(past, row, sizePast-1);
        if (t >= sizePast * dt) {
            
            PnlMat* temp = pnl_mat_create_from_zero(sizePast, option->size_);
            for (int i = 0; i < sizePast-1; i++) {
                pnl_mat_get_row(row, past, i);
                pnl_mat_set_row(temp, row, i);
            }
            int nearest_index = std::round(t / dt);
            if (std::fabs(nearest_index * dt - t) < 1E-12) {
                continue;
            } else {
                nearest_index = int (t / dt);
            }
            pnl_mat_get_row(row, path, nearest_index);
            pnl_mat_set_row(temp, row, sizePast-1);
            pnl_mat_free(&past);
            past = pnl_mat_copy(temp);
            pnl_mat_free(&temp);
            sizePast++;
        }
        // monteCarlo.price(past, t, newPrice, std_dev);
        monteCarlo.delta(past, t, newDelta, deltaStdDev);
        
        pnl_vect_clone(tempDelta, newDelta);
        pnl_vect_minus_vect(newDelta, delta);


        portfolioValue = portfolioValue * exp(model.r_ * tau) - pnl_vect_scalar_prod(newDelta, assetPrice);
        pnl_vect_clone(delta, tempDelta);
    }
    pnl_mat_get_row(assetPrice, path, path->m - 1);
    // Calcul du P&L final
    double finalPayoff = option->payoff(path);
    pnl = portfolioValue + pnl_vect_scalar_prod(delta, assetPrice) - finalPayoff;
    delete option;
    pnl_vect_free(&delta);
    pnl_vect_free(&newDelta);
    pnl_vect_free(&tempDelta);
    pnl_vect_free(&assetPrice);
    pnl_vect_free(&deltaStdDev);
    pnl_mat_free(&past);
    pnl_vect_free(&row);
}
