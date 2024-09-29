#include "BlackScholesModel.hpp"
#include "json_helper.hpp"
#include <cmath>
#include <iostream>

BlackScholesModel::BlackScholesModel(nlohmann::json j)
{
    double r;
    j.at("interest rate").get_to(r);
    double rho;
    j.at("correlation").get_to(rho);
    int size =  j.at("option size");
    PnlVect *sigma;
    j.at("volatility").get_to(sigma);
    if (sigma->size == 1 && size > 1) {
        pnl_vect_resize_from_scalar(sigma, size, GET(sigma, 0));
    }
    PnlVect *spot;
    j.at("spot").get_to(spot);
    if (spot->size == 1 && size > 1) {
        pnl_vect_resize_from_scalar(spot, size, GET(spot, 0));
    }

    this->size_ = size;
    this->r_ = r;
    this->rho_ = rho;
    this->sigma_ = pnl_vect_copy(sigma);
    this->spot_ = pnl_vect_copy(spot);

    // Creation of the Correlation matrice then compute Cholesky transformation
    this->L_ = pnl_mat_create_from_scalar(size_,size_,rho_);
    pnl_mat_set_diag (this->L_, 1, 0);
    pnl_mat_chol (this->L_);
}

void BlackScholesModel::asset(PnlMat* path, double T, int nbTimeSteps, PnlRng* rng){
    //Compute the deltaT
    double deltaT =    T/nbTimeSteps;

    // Create the Gi(i) matrice
    PnlMat* matNormalDistrubition = pnl_mat_create (size_,nbTimeSteps);
    pnl_mat_rng_normal(matNormalDistrubition,size_,nbTimeSteps,rng);

    //Compute the matrice of the multplicati of every Ld x Gi
    PnlMat* matLG = pnl_mat_mult_mat(L_,matNormalDistrubition);

    double simulatedSpotPrice= GET(spot_,0);
    pnl_mat_set(path,0,0,simulatedSpotPrice);

    //Loop on asset
    for (int d=0;d<size_;d++){
        double simulatedSpotPrice= GET(spot_,d);
        pnl_mat_set(path,0,d,simulatedSpotPrice);

        //Loop on sample
        for (int t =1;t<=nbTimeSteps;t++){
            double facteur = exp((r_-pow(GET(sigma_,d),2)/2)*deltaT + GET(sigma_,d)*sqrt(deltaT)*MGET(matLG,d,t-1));
            simulatedSpotPrice= simulatedSpotPrice*facteur;
            pnl_mat_set(path,t,d,simulatedSpotPrice);
    }
  }
}
    /**
     * Calcule une trajectoire du modèle connaissant le
     * passé jusqu' à la date t
     *
     * @param[out] path  contient une trajectoire du sous-jacent. La partie jusqu'à
     * l'instant t est données par la matrice past
     * @param[in] t date jusqu'à laquelle on connait la trajectoire.
     * t n'est pas forcément une date de discrétisation
     * @param[in] nbTimeSteps nombre de pas de constatation
     * @param[in] T date jusqu'à laquelle on simule la trajectoire
     * @param[in] past trajectoire réalisée jusqu'a la date t
     */
    void BlackScholesModel::asset(PnlMat* path, double t, double T, int nbTimeSteps, PnlRng* rng, const PnlMat* past){

      double deltaT =    T/nbTimeSteps;
      int numberOfKnownPrice = past->m;
      int rowOfLastKnownPrice = numberOfKnownPrice - 1;
      int newSample = nbTimeSteps - rowOfLastKnownPrice + 1;
  
      //TODO looks like its useless
      PnlVect* vectLastPrice = pnl_vect_create(size_);

      pnl_mat_get_row (vectLastPrice,past , rowOfLastKnownPrice);

      pnl_mat_set_subblock(path,past,0,0);
   
      // Create the Gi(i) matrice
      PnlMat* matNormalDistrubition = pnl_mat_create (size_,nbTimeSteps );
      pnl_mat_rng_normal(matNormalDistrubition,size_,nbTimeSteps,rng);

      //Compute the matrice of the multplicati of every Ld x Gi
      PnlMat* matLGi = pnl_mat_mult_mat(L_,matNormalDistrubition);

      //Loop on asset
      for (int j=0;j<size_;j++){

        double newDeltaT = deltaT * rowOfLastKnownPrice   - t;
        double facteur = exp((r_-pow(GET(sigma_,j),2)/2)*newDeltaT + GET(sigma_,j)*sqrt(newDeltaT)*MGET(matLGi,j,0));
        double simulatedSpotPrice = GET(vectLastPrice,j)*facteur;

        //Override
        pnl_mat_set(path,rowOfLastKnownPrice,j,simulatedSpotPrice);

        //Loop on sample(only if nbTimeStamp >1  i.e. for Asian)
        for (int i =rowOfLastKnownPrice+1;i<=nbTimeSteps;i++){
            double facteur = exp((r_-pow(GET(sigma_,j),2)/2)*deltaT + GET(sigma_,j)*sqrt(deltaT)*MGET(matLGi,j,i-rowOfLastKnownPrice));
            simulatedSpotPrice= simulatedSpotPrice*facteur;
            pnl_mat_set(path,i,j,simulatedSpotPrice);
      }
  }
    pnl_mat_free(&matNormalDistrubition);
    pnl_mat_free(&matLGi);
    pnl_vect_free(&vectLastPrice);
}


void BlackScholesModel::shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double h, double t, double timestep){
        int firstShift = (int)(t/timestep) +1 ;
        for(int timeIndex = firstShift; timeIndex < path->m; timeIndex++)
        {
            double current = MGET(path, timeIndex, d);
            current *= 1+h;
            pnl_mat_set(shift_path, timeIndex, d, current);
        }
    }
