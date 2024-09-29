#include <iostream>
#include <string>
#include <fstream>
#include "../ProfitAndLoss.hpp"
#include "nlohmann/json.hpp"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Wrong number of arguments. Use of the test : ./testPnl option.json marketData.dat" << std::endl;
        std::exit(0);
    };

    std::ifstream ifs(argv[1]);
    nlohmann::json j = nlohmann::json::parse(ifs);

    Option* option = OptionFactory::buildOption(j);
    std::cout << "Option Created" << std::endl;

    // Set the model
    BlackScholesModel model = BlackScholesModel(j);
    std::cout << "Model Created" << std::endl;

    char *file = argv[2];
    PnlMat* path = pnl_mat_create_from_file(file);
    double result = 0.0;
    // Display path 
    std::cout << "Computing PnL: " << std::endl;

    ProfitAndLoss::computePnl(path, result, model, option, j);

    // Print the option
    std::cout << "Pnl (erreur de couverture): " << std::endl;
    std::cout << result << std::endl;

}
