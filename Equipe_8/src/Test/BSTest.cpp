#include <iostream>
#include <string>
#include <fstream>
#include "../json_helper.hpp"
#include "../BlackScholesModel.hpp"


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments. Use of the test : ./OptionTest option.json expected.json" << std::endl;
        std::exit(0);
    };

    std::ifstream ifs(argv[1]);
    nlohmann::json j = nlohmann::json::parse(ifs);

    BlackScholesModel model = BlackScholesModel(j);

    // Print the option
    std::cout << "Model Correlation : " << std::endl;
    std::cout << model.rho_ << std::endl;
    std::cout << "Model Interest Rate : " << std::endl;
    std::cout << model.r_ << std::endl;
    for(int i = 0; i<model.size_; i++){
        // Display the spot
        std::cout << "Model Spot : " << std::endl;
        std::cout << GET(model.spot_, i) << std::endl;
        // Display the sigma
        std::cout << "Model Sigma : " << std::endl;
        std::cout << GET(model.sigma_, i) << std::endl;
    }
}
