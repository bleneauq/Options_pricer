#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "AsianOption.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments. Use of the test : ./OptionTest option.json expected.json" << std::endl;
        std::exit(0);
    };

    std::ifstream ifs(argv[1]);
    nlohmann::json jsonIn = nlohmann::json::parse(ifs);

    MonteCarlo monteCarlo = MonteCarlo::MonteCarlo();
}
