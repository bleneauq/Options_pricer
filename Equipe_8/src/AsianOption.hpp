#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Option.hpp"
class AsianOption: public Option
{
    /**
     * Calcule la valeur du payoff sur la trajectoire
     *
     * @param[in] path est une matrice de taille (N+1) x d
     * contenant une trajectoire du modèle telle que créée
     * par la fonction asset.
     * @return phi(trajectoire)
     */

    // Constructor inherited frolm Option.hpp
    public:
        double strike_;  /// strike
        PnlVect* weigths_;  /// payoff coefficients

        AsianOption(double T, PnlVect* weigths, double strike, int nbTimeSteps, int size);
        
        double payoff(const PnlMat* path);
};
