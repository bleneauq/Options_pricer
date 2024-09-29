#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <string>
#include <iostream>
#include <string>
#include <fstream>
#include "OptionFactory.hpp"



Option* OptionFactory::buildOption(nlohmann::json j) {
    // Set the option
    double strike;
    double T;
    j.at("maturity").get_to(T);
    int nbTimeSteps;
    j.at("timestep number").get_to(nbTimeSteps);
    int size =  j.at("option size");
    PnlVect *weigths;
    j.at("payoff coefficients").get_to(weigths);
    if (weigths->size == 1 && size > 1) {
        pnl_vect_resize_from_scalar(weigths, size, GET(weigths, 0));
    }
    std::string typeOption;
    typeOption = j.at("option type").get<std::string>();
    
    if (typeOption == "asian") {
        j.at("strike").get_to(strike);
        return new AsianOption(T, weigths, strike, nbTimeSteps, size);
    } else if (typeOption == "basket") {
        j.at("strike").get_to(strike);
        return new BasketOption(T, weigths, strike, nbTimeSteps, size);
    } else if (typeOption == "performance") {
        return new PerformanceOption(T, weigths, nbTimeSteps, size);
    } else {
            throw "Wrong option type!";
    }

}