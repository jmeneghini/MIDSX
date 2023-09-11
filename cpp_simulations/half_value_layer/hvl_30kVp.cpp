#include <MIDSX/Core.h>
#include <pybind11/embed.h>
#include <numeric>
#include <iomanip>
#include <omp.h>

namespace py = pybind11;

std::shared_ptr<DataAccessObject> setupDataService() {
    return std::make_shared<DataAccessObject>("data/data_sources/EPDL/EPDL.db");
}

std::vector<std::unique_ptr<Material>> initializeMaterials(const std::shared_ptr<DataAccessObject>& data_service) {
    std::vector<std::unique_ptr<Material>> materials;
//    materials.emplace_back(std::make_unique<Material>("Al", data_service));
    materials.emplace_back(std::make_unique<Material>("Air, Dry (near sea level)", data_service));
    return materials;
}

std::vector<std::shared_ptr<Tally>> initializeTallies() {
    std::vector<std::shared_ptr<Tally>> tallies;

    tallies.emplace_back(std::make_shared<DiscSurfaceTally>(
            Eigen::Vector3d(2, 2, 100),
            Eigen::Vector3d(0, 0, 1),
            1.0,
            QuantityContainerFactory::AllQuantities()));
    return tallies;
}

Eigen::MatrixXd processEnergySpectrum() {
    Eigen::MatrixXd energy_spectrum = SourceHelpers::readCSV("data/source_distributions/RQRM3_MO_MO_30kVp_E_spectrum.csv");
    energy_spectrum.col(0) = energy_spectrum.col(0).array() * 1000;  // Convert from keV to eV
    return energy_spectrum;
}

PhotonSource initializeSource() {
    auto energy_spectrum = processEnergySpectrum();

    PolyenergeticSpectrum poly_spectrum(energy_spectrum);
    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<PolyenergeticSpectrum>(poly_spectrum);
\
    std::unique_ptr<Directionality> directionality = std::make_unique<DiscIsotropicDirectionality>(
            DiscIsotropicDirectionality(Eigen::Vector3d(2, 2, 10),
                                        Eigen::Vector3d(0, 0, 1),
                                        0.1));

    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(
            PointGeometry(Eigen::Vector3d(2, 2, 0)));

    PhotonSource source(std::move(spectrum), std::move(directionality), std::move(geometry));
    return source;
}



#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
void runSimulation(PhotonSource& source, PhysicsEngine& physics_engine, int N_photons) {
    int j = 0;
#pragma omp parallel for
    for (int i = 0; i < N_photons; i++) {
        Photon photon = source.generatePhoton();
        physics_engine.transportPhoton(photon);
        if (i % (N_photons / 20) == 0) {
//            std::cout << "Progress: " << j << "%" << std::flush << "\r";
            j += 5;
        }
    }
}
#pragma clang diagnostic pop

void displayResults(const std::vector<std::shared_ptr<Tally>>& tallies, int N_photons, InteractionData& interaction_data) {
    std::vector<double> air_kerma_values;
//    Eigen::VectorXd energy_bin(1);
//    energy_bin << 30E3;
//    std::cout << energy_bin << std::endl;
    Eigen::VectorXd energy_bin = processEnergySpectrum().col(0);

    int i = 1;
    for (auto& tally : tallies) {
        auto quantity_container = tally->getQuantityContainer();
        quantity_container->processMeasurements();
        auto quantity = quantity_container->getTallyData();
        auto primary_air_kerma = quantity_container->getPrimaryAirKerma(interaction_data, 3, energy_bin);
        air_kerma_values.push_back(primary_air_kerma);

//        std::cout << "\nTally " << i << std::endl;
//        std::cout << "Number of total photons at detector " << i << ": " << quantity.number_of_particles << std::endl;
//        std::cout << "Number of primary photons at detector " << i << ": " << quantity.number_of_primary_particles << std::endl;
//        std::cout << "Number of secondary photons at detector " << i << ": " << quantity.number_of_secondary_particles << std::endl;
//        std::cout << "Primary air kerma at detector " << i << ": " << primary_air_kerma << std::endl;
        std::cout << std::setprecision(15) << primary_air_kerma << std::endl;
        i++;
    }

//    std::cout << "\nPrimary air kerma ratio: " << air_kerma_values[1] / air_kerma_values[0] << std::endl;

}

int main() {
    auto data_service = setupDataService();
    auto materials = initializeMaterials(data_service);
    auto tallies = initializeTallies();

    InteractionData interaction_data(std::move(materials), data_service);
    ComputationalDomain comp_domain("cpp_simulations/half_value_layer/hvl.json");
    PhysicsEngine physics_engine(comp_domain, interaction_data, tallies);



    PhotonSource source = initializeSource();

    runSimulation(source, physics_engine, 10000000);

    displayResults(tallies, 10000000, interaction_data);

    return 0;
}
