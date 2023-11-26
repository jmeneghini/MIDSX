#include <MIDSX/Core.h>
#include <pybind11/embed.h>
#include <numeric>
#include <iomanip>
#include <omp.h>

namespace py = pybind11;

std::vector<std::unique_ptr<SurfaceTally>> initializeSurfaceTallies() {
    std::vector<std::unique_ptr<SurfaceTally>> tallies = {};

    tallies.emplace_back(std::make_unique<DiscSurfaceTally>(
            Eigen::Vector3d(2, 2, 100),
            Eigen::Vector3d(0, 0, 1),
            1.0,
            SurfaceQuantityContainerFactory::AllQuantities()));
    return tallies;
}

Eigen::MatrixXd processEnergySpectrum() {
    Eigen::MatrixXd energy_spectrum = SourceHelpers::readCSV("data/source_distributions/RQRM3_MO_MO_30kVp_E_spectrum.csv");
    energy_spectrum.col(0) = energy_spectrum.col(0).array() * 1000;  // Convert from keV to eV
    return energy_spectrum;
}

PhotonSource initializeSource() {
    auto energy_spectrum = processEnergySpectrum();

//    PolyenergeticSpectrum poly_spectrum(energy_spectrum);
//    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<PolyenergeticSpectrum>(poly_spectrum);

    MonoenergeticSpectrum mono_spectrum(30E3);
    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<MonoenergeticSpectrum>(mono_spectrum);

    std::unique_ptr<Directionality> directionality = std::make_unique<DiscIsotropicDirectionality>(
            DiscIsotropicDirectionality(Eigen::Vector3d(2, 2, 10),
                                        Eigen::Vector3d(0, 0, 1),
                                        0.1));

    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(
            PointGeometry(Eigen::Vector3d(2, 2, 0)));

    PhotonSource source(std::move(spectrum), std::move(directionality), std::move(geometry));
    return source;
}

void displayResults(const std::vector<std::unique_ptr<SurfaceTally>>& surface_tallies, int N_photons, InteractionData& interaction_data) {
    auto quantity_container = surface_tallies[0]->getSurfaceQuantityContainer();
    auto vector_quantities = quantity_container.getVectorQuantities();
    auto count_quantities = quantity_container.getCountQuantities();

    // if mono
//    double primary_air_kerma = DerivedQuantity::getPrimaryAirKerma(quantity_container, interaction_data, 30E3, 0, false);
    // if poly
    Eigen::VectorXd energy_bin = processEnergySpectrum().col(0);
    double primary_air_kerma = DerivedQuantity::getPrimarySpectrumAirKerma(quantity_container, interaction_data, energy_bin, false);

    std::cout << "Number of Primary Photons: " << count_quantities.at(CountSurfaceQuantityType::NumberOfPhotons).getPrimaryValues().getCount() << std::endl;
    std::cout << "Primary Air Kerma: " << primary_air_kerma << std::endl;
}

int main() {
    auto surface_tallies = initializeSurfaceTallies();

    ComputationalDomain comp_domain("cpp_simulations/half_value_layer/hvl.json");
    InteractionData interaction_data = comp_domain.getInteractionData();
    PhysicsEngine physics_engine(comp_domain, interaction_data, {}, std::move(surface_tallies));



    PhotonSource source = initializeSource();

    int N_PHOTONS = 10000000;
    runSimulation(source, physics_engine, N_PHOTONS);

    displayResults(physics_engine.getSurfaceTallies(), N_PHOTONS, interaction_data);

    return 0;
}
