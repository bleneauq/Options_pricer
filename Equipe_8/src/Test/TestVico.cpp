#include <iostream>
#include <ctime>
#include <fstream>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "../OptionFactory.hpp"
#include "../MonteCarlo.hpp"



// int main(int argc, char** argv) {




// // int size=5;    /// nombre d'actifs du modèle
// // double r=0.02;     /// taux d'intérêt
// // double rho=0.5;     /// paramètre de corrélation
// // int nbTimeSteps = 10;
// // PnlVect* sigma = pnl_vect_create_from_scalar(size,0.2);
// // PnlVect* spot = pnl_vect_create_from_scalar(size,100);
// // PnlRng* rngfff = pnl_rng_create(PNL_RNG_MRGK3);
// // pnl_rng_sseed(rngfff, time(NULL));
// // PnlMat* path= pnl_mat_create_from_zero(nbTimeSteps+1,size);
// // double T = 1;
// // double t = 0.25;


// // BlackScholesModel BSM (size,r,rho,sigma,spot);
// // BSM.asset(path,T,nbTimeSteps,rngfff);
// //   std::cout << "\n";
// //   std::cout << "\n";
// // int nbAsset = 3;

// //   std::cout << "\n";
// //   std::cout << "\n";
// //   pnl_mat_print(path);


// // std::ifstream ifs(argv[1]);
// // nlohmann::json j = nlohmann::json::parse(ifs);
// // int nbTimeSteps = 3;
// // BlackScholesModel *BSM = BlackScholesModel(j);
// // PnlRng* rngfff = pnl_rng_create(PNL_RNG_MRGK3);
// // pnl_rng_sseed(rngfff, time(NULL));
// // //std::cout << BSM.size_;
// // std::cout << "\n";
// // MonteCarlo MC = MonteCarlo(mBSMod, , PnlRng* rng, nlohmann::json j)





// // PnlMat* path= pnl_mat_create_from_zero(nbTimeSteps+1,BSM.size_);
// // pnl_mat_print(path);
// // double T = 1.5;
// // double t = 0.98;
// // PnlMat* past = pnl_mat_create_from_scalar(nbTimeSteps,2,100);
// // pnl_mat_set(past,2,0,120);
// // pnl_mat_set(past,2,1,115);
// //   std::cout << "\n";
// //   std::cout << "\n";
// // pnl_mat_print(past);












//  if (argc != 3) {
//         std::cerr << "Wrong number of arguments. Exactly 2 argument is required" << std::endl;
//         std::exit(0);
//     };
//     std::ifstream ifs(argv[1]);
//     nlohmann::json j = nlohmann::json::parse(ifs);

//     // Set the model
//     BlackScholesModel model = BlackScholesModel(j);
//     std::cout << "Model Created" << std::endl;

//     // Set the option
//     Option* option = OptionFactory::buildOption(j);
//     std::cout << "Option Created" << std::endl;

//     PnlRng* rng;
//     rng = pnl_rng_create(PNL_RNG_MERSENNE);
//     pnl_rng_sseed(rng, time(NULL));
//     // Set the MonteCarlo Approximator
//     MonteCarlo monteCarlo = MonteCarlo( 
//         &model,
//         option,
//         rng,
//         j);
    
//     std::cout << "MonteCarlo Created" << std::endl;

//     // Display Expected Values 
//     std::ifstream ifs2(argv[2]);
//     nlohmann::json j2 = nlohmann::json::parse(ifs2);
//     std::cout << "Expected Price: " << j2.at("price") << std::endl;
//     std::cout << "Expected Standard deviation: " << j2.at("priceStdDev") << std::endl;

//     // Compute the price
//     double price, std_dev;
//     monteCarlo.price(price, std_dev);
//     std::cout << "Price: " << price << std::endl;
//     std::cout << "Standard deviation: " << std_dev << std::endl;

//     // Compute Delta
//     PnlVect* delta = pnl_vect_create(option->size_);
//     PnlVect* deltaStdDev = pnl_vect_create(option->size_);
//     monteCarlo.delta(delta, deltaStdDev);
//     std::cout << "Delta: " << std::endl;
//     pnl_vect_print(delta);
//     std::cout << "Standard deviation of each d-Delta estimator: " << std::endl;
//     pnl_vect_print(deltaStdDev);























// //PnlMat* path2= pnl_mat_create_from_zero(nbTimeSteps+1,nbAsset);
// //const PnlMat* block = pnl_mat_create_from_scalar(3, nbAsset, 100);
// //pnl_mat_print(block);
//   std::cout << "\n";
//   std::cout << "\n";
// //BSM.asset(path2,t,T,nbTimeSteps,rngfff,block);


//   std::cout << "\n";
//   std::cout << "\n";
//   std::cout << "AFTER BSM ASSET METHOD";
//   std::cout << "\n";
//   std::cout << "\n";
// //pnl_mat_print(path2);



// } 


int
main(int argc, char** argv){
    if (argc != 3) {
        std::cerr << "Wrong number of arguments. Exactly 2 argument is required" << std::endl;
        std::exit(0);
    };
    std::ifstream ifs(argv[1]);
    nlohmann::json j = nlohmann::json::parse(ifs);

    // Set the model
    BlackScholesModel model = BlackScholesModel(j);
    std::cout << "Model Created" << std::endl;

    // Set the option
    Option* option = OptionFactory::buildOption(j);
    std::cout << "Option Created" << std::endl;

    PnlRng* rng;
    rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    // Set the MonteCarlo Approximator
    MonteCarlo monteCarlo = MonteCarlo( 
        &model,
        option,
        rng,
        j);
    
    std::cout << "MonteCarlo Created" << std::endl;

    // Display Expected Values 
    std::ifstream ifs2(argv[2]);
    nlohmann::json j2 = nlohmann::json::parse(ifs2);
    std::cout << "Expected Price: " << j2.at("price") << std::endl;
    std::cout << "Expected Standard deviation: " << j2.at("priceStdDev") << std::endl;

    // Compute the price
    double price, std_dev;
    
    //Add the parameters to call the price at t MC method
    double t = 0.4;
    PnlMat *past = pnl_mat_create_from_zero(3, 2);
    pnl_mat_set(past, 0, 0, 100);
    pnl_mat_set(past, 0, 1, 100);

    pnl_mat_set(past, 1, 0, 150);
    pnl_mat_set(past, 1, 1, 150);

    pnl_mat_set(past, 2, 0, 200);
    pnl_mat_set(past, 2, 1, 200);

  

    //monteCarlo.price(past, t, price, std_dev);
    monteCarlo.price(price, std_dev);
    std::cout << "Price: " << price << std::endl;
    std::cout << "Standard deviation: " << std_dev << std::endl;

    // Compute Delta
    PnlVect* delta = pnl_vect_create(option->size_);
    PnlVect* deltaStdDev = pnl_vect_create(option->size_);
    monteCarlo.delta(past, t, delta, deltaStdDev);
    //monteCarlo.delta(delta, deltaStdDev);
    std::cout << "Delta: " << std::endl;
    pnl_vect_print(delta);
    std::cout << "Standard deviation of each d-Delta estimator: " << std::endl;
    pnl_vect_print(deltaStdDev);

    // Printe  the Delta :

    return 0;
}
