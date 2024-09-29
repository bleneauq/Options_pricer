#include "MonteCarlo.hpp"
#include "OptionFactory.hpp"
#include <nlohmann/json.hpp>
#include "pnl/pnl_matrix.h"

class ProfitAndLoss {
    /**
     * Calcule la valeur du profit&loss de notre portefeuille
     *
     * @param[in] path est une matrice de taille (N+1) x d
     * contenant une trajectoire du modèle récupérée dans les
     * fichiers de test .dat
     * @param[in] model est notre modèle de BlackScholes 
     * @param[in] option est l'option que nous évaluons
     */
    public:
        void static computePnl(PnlMat* path, double& result, BlackScholesModel model, Option* option, nlohmann::json j);
};
