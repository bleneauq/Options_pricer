#include <iostream>
#include <string>
#include <fstream>
#include "../json_helper.hpp"
#include "../Option.hpp"
#include "../OptionFactory.hpp"


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments. Use of the test : ./OptionTest option.json expected.json" << std::endl;
        std::exit(0);
    };

    std::ifstream ifs(argv[1]);
    nlohmann::json j = nlohmann::json::parse(ifs);

    Option* option = OptionFactory::buildOption(j);

    // Print the option
    std::cout << "Option Maturity : " << std::endl;
    std::cout << option->T_ << std::endl;
    std::cout << "Option Strike : " << std::endl;
    std::cout << option->strike_ << std::endl;
    std::cout << "Option nbTimeSteps : " << std::endl;
    std::cout << option->nbTimeSteps_ << std::endl;

}
