#include <iostream>
#include <string>
#include <fstream>
#include "../json_helper.hpp"
#include "../Option.hpp"
#include "../OptionFactory.hpp"
#include "../BlackScholesModel.hpp"
#include "../MonteCarlo.hpp"


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments. Use of the test : ./OptionTest option.json expected.json" << std::endl;
        std::exit(0);
    };

    std::ifstream ifs(argv[1]);
    nlohmann::json j = nlohmann::json::parse(ifs);

    BlackScholesModel model = BlackScholesModel(j);
    Option* option = OptionFactory::buildOption(j);
    MonteCarlo monteCarlo = MonteCarlo(
        &model,
        option,
        pnl_rng_create(PNL_RNG_MERSENNE),
        j);

    
}
