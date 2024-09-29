#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <string>
#include "json_helper.hpp"
#include "Option.hpp"
#include "AsianOption.hpp"
#include "BasketOption.hpp"
#include "PerformanceOption.hpp"

/// \brief Classe OptionFactory abstraite
class OptionFactory
{
    /**
     * Calcule la valeur du payoff sur la trajectoire
     *
     * @param[in] typeOption représente le type de l'option capturé
     * dans le json
     * @return option
     */
    public:
        static Option* buildOption(nlohmann::json j);
};
