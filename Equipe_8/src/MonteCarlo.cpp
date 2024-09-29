#include <cmath>
#include "MonteCarlo.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include <iostream>
#include <fstream>
double
Inverse(double input)
{
    return 1 / input;
}

double
SquareInverse(double input)
{
    return 1 / (input*input);
}

// Implement Constructor correctly
MonteCarlo::MonteCarlo(BlackScholesModel* mod, Option* opt, PnlRng* rng, nlohmann::json j)
{
    double fdStep;
    fdStep = j.at("fd step");
    fdStep_ = fdStep;
    long nbSamples;
    j.at("sample number").get_to(nbSamples);
  
    mod_ = mod;
    opt_ = opt;
    rng_ = rng;
    fdStep_ = fdStep;
    nbSamples_ = nbSamples;
}

/**
 * Calcule le prix de l'option à la date 0
 *
 * @param[out] prix valeur de l'estimateur Monte Carlo
 * @param[out] std_dev écart type de l'estimateur
 */

void MonteCarlo::price(double& prix, double& std_dev)
{
  double T = this->opt_->T_;
  int nbTimeSteps = this->opt_->nbTimeSteps_;
  double payoff;
  double totalPrice = 0;
  double squarePayoff = 0;
  PnlMat *path = pnl_mat_create_from_zero(this->opt_->nbTimeSteps_+1,this->mod_->size_);

  for(long M = 0; M<this->nbSamples_; M++)
  {
    this->mod_->asset(path, T, nbTimeSteps, this->rng_);
    payoff = this->opt_->payoff(path);
    totalPrice += payoff;
    squarePayoff += payoff*payoff;
  }
  double payoffOption = totalPrice/this->nbSamples_;
  double variance = exp(-2*this->mod_->r_*T)*((squarePayoff/this->nbSamples_) - (payoffOption*payoffOption));
  // Print the Price
  prix = exp(-this->mod_->r_*T)*payoffOption;
  std_dev = sqrt(variance)/sqrt(this->nbSamples_);
  pnl_mat_free(&path);
}

/**
 * Calcule le prix de l'option à la date t
 *
 * @param[in]  past contient la trajectoire du sous-jacent
 * jusqu'à l'instant t
 * @param[in] t date à laquelle le calcul est fait
 * @param[out] prix contient le prix
 * @param[out] std_dev contient l'écart type de l'estimateur
 */
void MonteCarlo::price(const PnlMat* past, double t, double& prix, double& std_dev)
{
  double T = this->opt_->T_;
  int nbTimeSteps = this->opt_->nbTimeSteps_;
  double payoff;
  double totalPrice = 0;
  double squarePayoff = 0;
  PnlMat *path = pnl_mat_create_from_zero(this->opt_->nbTimeSteps_+1,this->mod_->size_);
  for(long M = 0; M<this->nbSamples_; M++)
  {
    this->mod_->asset(path, t, T, nbTimeSteps, this->rng_,past);
    payoff = this->opt_->payoff(path);
    totalPrice += payoff;
    squarePayoff += payoff*payoff;
  }
  double payoffOption = totalPrice/this->nbSamples_;
  double variance = exp(-2*this->mod_->r_*T)*((squarePayoff/this->nbSamples_) - (payoffOption*payoffOption));
  // Print the Price
  prix = exp(-this->mod_->r_*T)*payoffOption;
  std_dev = sqrt(variance)/sqrt(this->nbSamples_);
  pnl_mat_print(path);
  pnl_mat_free(&path);
}

/**
 * Calcule le delta de l'option à la date t
 *
 * @param[in] past contient la trajectoire du sous-jacent
 * jusqu'à l'instant t
 * @param[in] t date à laquelle le calcul est fait
 * @param[out] delta contient le vecteur de delta
 * @param[out] std_dev contient l'écart type de l'estimateur
 */
void
MonteCarlo::delta(const PnlMat* past, double t, PnlVect* delta, PnlVect* std_dev)
{
  PnlMat* path = pnl_mat_create_from_zero(this->opt_->nbTimeSteps_ + 1, this->mod_->size_);
  PnlMat* shiftedPath = pnl_mat_copy(path);
  double payoffShiftPlus;
  double payoffShiftMinus;
  double h = this->fdStep_;
  double timestep = this->opt_->T_ / (double)this->opt_->nbTimeSteps_;

  pnl_vect_set_all(std_dev, 0);
  pnl_vect_set_all(delta, 0);

  for (long M = 0; M < this->nbSamples_; M++) {
      // Complete a price trajectory
      this->mod_->asset(path, t, this->opt_->T_, this->opt_->nbTimeSteps_, this->rng_, past);

      // Iterate over all the assets to compute each Delta
      for (int d = 0; d < this->mod_->size_; d++) {
          shiftedPath = pnl_mat_copy(path);
          this->mod_->shiftAsset(shiftedPath, path, d, h, t, timestep);
          payoffShiftPlus = this->opt_->payoff(shiftedPath);

          this->mod_->shiftAsset(shiftedPath, path, d, -h, t, timestep);
          payoffShiftMinus = this->opt_->payoff(shiftedPath);

          double temp = GET(delta, d);
          double tempSquare = GET(std_dev, d);

          double diff = payoffShiftPlus - payoffShiftMinus;

          pnl_vect_set(delta, d, temp + diff);
          pnl_vect_set(std_dev, d, tempSquare + diff * diff);
      }
  }
  // Keeps a copy of the delta values only
  PnlVect* copyDelta = pnl_vect_copy(delta);

  // Set of the delta pointer
  double multiple = (exp(-(this->opt_->T_ - t) * this->mod_->r_)) / (2 * this->nbSamples_ * h);
  PnlVect* multiplier = pnl_vect_create_from_scalar(this->mod_->size_, multiple);
  PnlVect* inversed = pnl_vect_create_from_scalar(this->mod_->size_, 1);
  double (*invFunction)(double);
  invFunction = Inverse;
  pnl_vect_map(inversed, this->mod_->spot_, invFunction);
  pnl_vect_mult_vect_term(delta, multiplier);
  pnl_vect_mult_vect_term(delta, inversed);

  // Multiply the stdDev vector
  multiple = (exp(-2 * (this->opt_->T_ - t) * this->mod_->r_)) / (4 * this->nbSamples_ * h * h);
  pnl_vect_set_all(multiplier, multiple);
  double (*squareInvFunction)(double);
  squareInvFunction = SquareInverse;
  pnl_vect_map(inversed, this->mod_->spot_, squareInvFunction);
  pnl_vect_mult_vect_term(multiplier, inversed);

  // Get the square of the sum of the delta
  pnl_vect_mult_vect_term(copyDelta, copyDelta);
  pnl_vect_mult_scalar(copyDelta, 1 / (double)this->nbSamples_);
  pnl_vect_minus_vect(std_dev, copyDelta);
  pnl_vect_mult_vect_term(std_dev, multiplier);
  pnl_vect_map(std_dev, std_dev, sqrt);
  pnl_vect_div_scalar(std_dev, sqrt((double)this->nbSamples_));

  pnl_vect_free(&multiplier);
  pnl_vect_free(&inversed);
  pnl_mat_free(&path);
  pnl_mat_free(&shiftedPath);
}


/**
 * Calcule le delta de l'option à la date 0
 *
 * @param[out] delta contient le vecteur de delta
 * @param[out] std_dev contient l'écart type de l'estimateur
 */
void MonteCarlo::delta(PnlVect* delta, PnlVect* std_dev)
{ 
  double T = this->opt_->T_;
  int nbTimeSteps = this->opt_->nbTimeSteps_;
  double payoffShiftPlus;
  double payoffShiftMinus;
  double totalPrice = 0;
  double squarePayoff = 0;
  PnlMat *path = pnl_mat_create_from_zero(this->opt_->nbTimeSteps_+1,this->mod_->size_);
  PnlMat *shiftedPath = pnl_mat_copy(path);

  pnl_vect_set_all(delta, 0);
  pnl_vect_set_all(std_dev, 0);

  double h = this->fdStep_;
  double timestep = T / (double)nbTimeSteps;

  for(long M = 0; M<this->nbSamples_; M++)
  {
    //Initialize a price trajectory
    this->mod_->asset(path, T, nbTimeSteps, this->rng_);
    
    //Iterate over all the assets to compute each Delta
    for(int d = 0; d<this->mod_->size_; d++)
    {
      shiftedPath = pnl_mat_copy(path);
      double nullDate = 0;
      this->mod_->shiftAsset(shiftedPath, path, d, h, nullDate, timestep);
      payoffShiftPlus = this->opt_->payoff(shiftedPath);

      this->mod_->shiftAsset(shiftedPath, path, d, -h, nullDate, timestep);
      payoffShiftMinus = this->opt_->payoff(shiftedPath);

      double temp = GET(delta, d);
      double tempSquare = GET(std_dev, d);

      double diff = payoffShiftPlus - payoffShiftMinus;

      pnl_vect_set(delta, d, temp + diff);
      pnl_vect_set(std_dev, d, tempSquare + diff*diff);
    }
  }
  //Keeps a copy of the delta values only
  PnlVect *copyDelta = pnl_vect_copy(delta);

  // Set of the delta pointer
  double multiple = (exp(- T * this->mod_->r_))/(2 * this->nbSamples_ * h);
  PnlVect *multiplier = pnl_vect_create_from_scalar(this->mod_->size_, multiple);
  PnlVect *inversed = pnl_vect_create_from_scalar(this->mod_->size_, 1);
  double (*invFunction)(double);
  invFunction = Inverse;
  pnl_vect_map(inversed, this->mod_->spot_, invFunction);
  pnl_vect_mult_vect_term(delta, inversed);
  pnl_vect_mult_vect_term(delta, multiplier);

  //Multiply the stdDev vector
  multiple = (exp(- 2 * T * this->mod_->r_))/(4 * this->nbSamples_ * pow(h, 2.0));
  pnl_vect_set_all(multiplier, multiple);
  double (*squareInvFunction)(double);
  squareInvFunction = SquareInverse;
  pnl_vect_map(inversed, this->mod_->spot_, squareInvFunction);
  pnl_vect_mult_vect_term(multiplier, inversed);

  //Get the square of the sum of the delta
  pnl_vect_mult_vect_term(copyDelta, copyDelta);
  pnl_vect_mult_scalar(copyDelta, 1/(double)this->nbSamples_);
  pnl_vect_minus_vect(std_dev, copyDelta);
  pnl_vect_mult_vect_term(std_dev, multiplier);
  pnl_vect_map(std_dev, std_dev, sqrt);
  pnl_vect_div_scalar(std_dev, sqrt((double)this->nbSamples_));

  pnl_mat_free(&path);
  pnl_mat_free(&shiftedPath);
  pnl_vect_free(&inversed);
  pnl_vect_free(&multiplier);
};
