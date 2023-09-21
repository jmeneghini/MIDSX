#include "Core/derived_quantities.h"

double DerivedQuantity::getPrimaryFluence(SurfaceQuantityContainer &surface_quantity_container, double energy, double energy_width, bool is_cosine_weighted) {
    // fluence (E +- delta E / 2) = dN/dE = total number of photons in energy range / area of surface  [units: 1/cm^2]
    double total = 0;
    VectorValue incident_energies;

    try {
        incident_energies = surface_quantity_container.getVectorQuantities().at(VectorSurfaceQuantityType::IncidentEnergy).getPrimaryValues();
    } catch (std::out_of_range& e) {
        throw std::runtime_error("SurfaceQuantityContainer does not contain incident energy vector quantity required for cosine-weighted fluence calculation");
    }
    // get vector
    std::vector<double> incident_energies_vector;
    incident_energies_vector = incident_energies.getVector();

    if (is_cosine_weighted) {
        // sum of entrance cosine
        VectorValue entrance_cosines;
        try {
            entrance_cosines = surface_quantity_container.getVectorQuantities().at(VectorSurfaceQuantityType::EntranceCosine).getPrimaryValues();
        } catch (std::out_of_range& e) {
            throw std::runtime_error("SurfaceQuantityContainer does not contain entrance cosine vector quantity required for cosine-weighted fluence calculation");
        }
        // get vectors
        std::vector<double> entrance_cosines_vector = entrance_cosines.getVector();

        for (int i = 0; i < incident_energies_vector.size(); i++) {
            if (incident_energies_vector[i] >= energy - energy_width/2 && incident_energies_vector[i] <= energy + energy_width/2) {
                total += entrance_cosines_vector[i];
            }
        }

    } else {
        for (int i = 0; i < incident_energies_vector.size(); i++) {
            if (incident_energies_vector[i] >= energy - energy_width/2 && incident_energies_vector[i] <= energy + energy_width/2) {
                total += 1;
            }
        }
    }
    return total / surface_quantity_container.getArea();
}

double DerivedQuantity::getPrimaryAirKerma(SurfaceQuantityContainer &surface_quantity_container, InteractionData &interaction_data,
                                           double energy, double energy_width, bool is_cosine_weighted) {
    // AK(E +- delta E / 2) = E * fluence(E +- delta E / 2) * (mu_en(E)/rho)  [units: eV/g]
    int AIR_INDEX = 3;
    auto air_data = interaction_data.getMaterial(AIR_INDEX).getData();
    double mu_en = air_data->interpolateMassEnergyAbsorptionCoefficient(energy); // [units: cm^2/g]
    double fluence = getPrimaryFluence(surface_quantity_container, energy, energy_width, is_cosine_weighted); // [units: 1/cm^2]
    return energy * fluence * mu_en; // [units: eV/g]
}

double DerivedQuantity::getPrimarySpectrumAirKerma(SurfaceQuantityContainer &surface_quantity_container,
                                                   InteractionData &interaction_data, Eigen::VectorXd energy_spectrum,
                                                   bool is_cosine_weighted) {
    // assumes midpoint evenly spaced energy bins
    double energy_width = energy_spectrum[1] - energy_spectrum[0];
    double total = 0;
    for (int i = 0; i < energy_spectrum.size(); i++) {
        total += getPrimaryAirKerma(surface_quantity_container, interaction_data, energy_spectrum[i], energy_width, is_cosine_weighted);
    }
    return total;
}
