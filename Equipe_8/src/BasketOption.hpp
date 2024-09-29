#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"

class BasketOption: public Option
{
    public :
      double strike_;  /// strike
      PnlVect* weigths_;  /// payoff coefficients
    // Constructor
      BasketOption(double T, PnlVect* weigths, double strike, int nbTimeSteps, int size);
      
      double payoff(const PnlMat* path);
};
