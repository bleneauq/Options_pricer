#include <iostream>
#include <ctime>
#include <fstream>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "nlohmann/json.hpp"
#include "OptionFactory.hpp"
#include "MonteCarlo.hpp"
#include "PricingResults.hpp"

using namespace std;

int
main(int argc, char** argv){
    if (argc != 2) {
        std::cerr << "Wrong number of arguments. Exactly one argument is required" << std::endl;
        std::exit(0);
    };
    std::ifstream ifs(argv[1]);
    nlohmann::json j = nlohmann::json::parse(ifs);

    // Set the model
    BlackScholesModel model = BlackScholesModel(j);

    // Set the option
    Option* option = OptionFactory::buildOption(j);

    PnlRng* rng;
    rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
    // Set the MonteCarlo Approximator
    MonteCarlo monteCarlo = MonteCarlo( 
        &model,
        option,
        rng,
        j);

    // Don't Change This 
    double price, std_dev;
    monteCarlo.price(price, std_dev);
    PnlVect* delta = pnl_vect_create(option->size_);
    PnlVect* delta_std_dev = pnl_vect_create(option->size_);
    monteCarlo.delta(delta, delta_std_dev);

    PricingResults res(price, std_dev, delta, delta_std_dev);
    std::cout << res << std::endl;
}
