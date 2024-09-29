#include "Option.hpp"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class PerformanceOption: public Option
{
    
    // Constructor
    public:
        PnlVect* weigths_;  /// payoff coefficients
        PerformanceOption(double T, PnlVect* weigths, int nbTimeSteps, int size);

        double payoff(const PnlMat* path);
};
