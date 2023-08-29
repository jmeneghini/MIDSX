#include <MIDSX/Core.h>
#include <pybind11/embed.h>
#include <numeric>
#include <omp.h>

namespace py = pybind11;

std::shared_ptr<DataAccessObject> setupDataService() {
    return std::make_shared<DataAccessObject>("data/data_sources/EPDL/EPDL.db");
}

std::vector<std::unique_ptr<Material>> initializeMaterials(const std::shared_ptr<DataAccessObject>& data_service) {
    std::vector<std::unique_ptr<Material>> materials;
    materials.emplace_back(std::make_unique<Material>("Al", data_service));
    materials.emplace_back(std::make_unique<Material>("Air, Dry (near sea level)", data_service));
    return materials;
}

std::vector<std::shared_ptr<Tally>> initializeTallies() {
    std::vector<std::shared_ptr<Tally>> tallies;

    tallies.emplace_back(std::make_shared<DiscSurfaceTally>(
            Eigen::Vector3d(20, 20, 0),
            Eigen::Vector3d(0, 0, 1),
            0.1,
            QuantityContainerFactory::AllQuantities()));

    tallies.emplace_back(std::make_shared<DiscSurfaceTally>(
            Eigen::Vector3d(20, 20, 0.2273),
            Eigen::Vector3d(0, 0, 1),
            0.1,
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

//    PolyenergeticSpectrum poly_spectrum(energy_spectrum);
    MonoenergeticSpectrum mono_spectrum(30E3);
    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<MonoenergeticSpectrum>(mono_spectrum);

    std::unique_ptr<Directionality> directionality = std::make_unique<BeamDirectionality>(
            BeamDirectionality(Eigen::Vector3d(20, 20, 1)));
//    std::unique_ptr<Directionality> directionality = std::make_unique<RectangularIsotropicDirectionality>(
//            RectangularIsotropicDirectionality(Eigen::Vector3d(0, 0, 180),
//                                               Eigen::Vector3d(39.0, 0, 0),
//                                               Eigen::Vector3d(0, 39.0, 0)));
    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(PointGeometry(Eigen::Vector3d(20, 20, 0)));

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
            std::cout << "Progress: " << j << "%" << std::flush << "\r";
            j += 5;
        }
    }
}
#pragma clang diagnostic pop

void displayResults(VoxelGrid& voxel_grid, const std::vector<std::shared_ptr<Tally>>& tallies, int N_photons, InteractionData& interaction_data) {
    std::vector<double> air_kerma_values;

    int i = 1;
    for (auto& tally : tallies) {
        auto quantity_container = tally->getQuantityContainer();
        quantity_container->processMeasurements();
        auto quantity = quantity_container->getTallyData();
        auto number_of_primary_particles = quantity.number_of_primary_particles;
        air_kerma_values.push_back(number_of_primary_particles);

        std::cout << "\nTally " << i << std::endl;
        std::cout << "Number of total photons at detector " << i << ": " << quantity.number_of_particles << std::endl;
        std::cout << "Number of primary photons at detector " << i << ": " << quantity.number_of_primary_particles << std::endl;
        i++;
    }

    std::cout << "\nPrimary air kerma ratio: " << air_kerma_values[1] / air_kerma_values[0] << std::endl;

}

int main() {
    auto data_service = setupDataService();
    auto materials = initializeMaterials(data_service);
    auto tallies = initializeTallies();

    InteractionData interaction_data(std::move(materials), data_service);
    VoxelGrid voxel_grid("data/voxels/al_hvl_30keV.nii.gz");
    PhysicsEngine physics_engine(voxel_grid, interaction_data, tallies);



    PhotonSource source = initializeSource();

    runSimulation(source, physics_engine, 1000000);

    displayResults(voxel_grid, tallies, 1000000, interaction_data);

    return 0;
}
