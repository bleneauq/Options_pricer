#include <iostream>
#include <ctime>
#include <fstream>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "../OptionFactory.hpp"
#include "../MonteCarlo.hpp"

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
    
    
    const char* file = argv[2];
    PnlMat *mat = pnl_mat_create_from_file(file);

    double deltaT =   option->T_/option->nbTimeSteps_;
    pnl_mat_resize(mat,mat->m - 1,mat->n);
    



    // Read a .dat using pnl_mat_create_from_file (const char ∗file)


    // Print the matrix on the terminal using pnl_mat_print
    pnl_mat_print(mat);
    std::cout << "OUI JE SUIS RACISTE: " << std::endl;
    std::cout << "OUIIIIIIIIIIIIIIIIIIIIIIIIIIII: " << std::endl;
    double price, std_dev;
    monteCarlo.price(price, std_dev);
    std::cout << "Price: " << price << std::endl;
    std::cout << "Standard deviation: " << std_dev << std::endl; 
    monteCarlo.price(mat, option->T_-deltaT*1.0001,price, std_dev);
    std::cout << "Price: " << price << std::endl;
    std::cout << "Standard deviation: " << std_dev << std::endl;
    
    exit(0);
}