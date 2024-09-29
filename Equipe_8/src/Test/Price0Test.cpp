#include <iostream>
#include <ctime>
#include <fstream>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "../OptionFactory.hpp"
#include "../MonteCarlo.hpp"


using namespace std;

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
    monteCarlo.price(price, std_dev);
    std::cout << "Price: " << price << std::endl;
    std::cout << "Standard deviation: " << std_dev << std::endl;

    // Compute Delta
    PnlVect* delta = pnl_vect_create(option->size_);
    PnlVect* deltaStdDev = pnl_vect_create(option->size_);
    monteCarlo.delta(delta, deltaStdDev);
    std::cout << "Delta: " << std::endl;
    pnl_vect_print(delta);
    std::cout << "Standard deviation of each d-Delta estimator: " << std::endl;
    pnl_vect_print(deltaStdDev);

    // Printe  the Delta :

    return 0;
}
