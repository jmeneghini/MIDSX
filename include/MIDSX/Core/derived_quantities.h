#ifndef RADIOGRAPHY_DERIVED_QUANTITIES_H
#define RADIOGRAPHY_DERIVED_QUANTITIES_H

#include "volume_tally.h"
#include "tally_data.h"
#include "surface_tally.h"
#include <variant>

namespace DerivedQuantity{
    double getPrimaryFluence(SurfaceQuantityContainer& surface_quantity_container, double energy, double energy_width, bool is_cosine_weighted = false);

    double getPrimaryAirKerma(SurfaceQuantityContainer& surface_quantity_container, InteractionData& interaction_data,
                              double energy, double energy_width, bool is_cosine_weighted = false);

    double getPrimarySpectrumAirKerma(SurfaceQuantityContainer& surface_quantity_container, InteractionData& interaction_data,
                                    Eigen::VectorXd energy_spectrum, bool is_cosine_weighted = false);
}

#endif //RADIOGRAPHY_DERIVED_QUANTITIES_H
