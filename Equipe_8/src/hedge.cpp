#include <iostream>
#include <string>
#include <fstream>
#include "ProfitAndLoss.hpp"
#include "nlohmann/json.hpp"
#include "HedgingResults.hpp"

using namespace std;

int
main(int argc, char** argv){
    if (argc != 3) {
        std::cerr << "Wrong number of arguments. Exactly 2 arguments are required" << std::endl;
        std::exit(0);
    };

    char *file = argv[1];
    PnlMat* path = pnl_mat_create_from_file(file);

    std::ifstream ifs(argv[2]);
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
    double price, std_dev, erreur_couverture;
    monteCarlo.price(price, std_dev);
    
    ProfitAndLoss::computePnl(path, erreur_couverture, model, option, j);
    HedgingResults res(price, std_dev, erreur_couverture);
    std::cout << res << std::endl;
    return 0;
}