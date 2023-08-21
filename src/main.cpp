#include "source.h"
#include "pybind11/embed.h"
#include "interaction_data.h"
#include "probability_dist.h"
#include <matplot/matplot.h>
#include "voxel_grid.h"
#include "physics_engine.h"
#include "detector.h"
#include "tally.h"
#include "source.h"
#include <omp.h>

namespace plt = matplot;
namespace py = pybind11;

std::shared_ptr<DataAccessObject> setupDataService() {
    return std::make_shared<DataAccessObject>("data/data_sources/EPDL/EPDL.db");
}

std::vector<std::shared_ptr<Material>> initializeMaterials(const std::shared_ptr<DataAccessObject>& data_service) {
    std::vector<std::shared_ptr<Material>> materials;
    materials.emplace_back(std::make_shared<Material>("Tissue, Soft (ICRU-46)", data_service));
    materials.emplace_back(std::make_shared<Material>("Air, Dry (near sea level)", data_service));
    return materials;
}

std::vector<std::shared_ptr<Tally>> initializeTallies() {
    std::vector<std::shared_ptr<Tally>> tallies;

    tallies.emplace_back(std::make_shared<RectangularSurfaceTally>(
            Eigen::Vector3d(0, 0, 155),
            Eigen::Vector3d(39.0, 0, 0),
            Eigen::Vector3d(0, 39.0, 0),
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
    MonoenergeticSpectrum mono_spectrum(56.4E3);
    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<MonoenergeticSpectrum>(mono_spectrum);

//    std::unique_ptr<Directionality> directionality = std::make_unique<BeamDirectionality>(BeamDirectionality(detector_position));
    std::unique_ptr<Directionality> directionality = std::make_unique<RectangularIsotropicDirectionality>(
            RectangularIsotropicDirectionality(Eigen::Vector3d(0, 0, 180),
                                               Eigen::Vector3d(39.0, 0, 0),
                                               Eigen::Vector3d(0, 39.0, 0)));
    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(PointGeometry(Eigen::Vector3d(39.0/2, 39.0/2, 0)));

    PhotonSource source(std::move(spectrum), std::move(directionality), std::move(geometry));
    return source;
}



#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
void runSimulation(PhotonSource& source, PhysicsEngine& physics_engine, int N_photons) {
    int j = 0;
#pragma omp parallel for private(source)
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

void displayResults(VoxelGrid& voxel_grid, const Detector& detector, const std::vector<std::shared_ptr<Tally>>& tallies, int N_photons, InteractionData& interaction_data) {
    int i = 1;
    for (auto& tally : tallies) {
        auto quantity_container = tally->getQuantityContainer();
        quantity_container->processMeasurements();
        auto quantity = quantity_container->getTallyData();
//        auto primary_air_kerma = quantity_container->getPrimaryAirKerma(interaction_data, 3, energy_spectrum.col(0));

        std::cout << "Number of total photons at detector " << i << ": " << quantity.number_of_particles << std::endl;
        std::cout << "Number of primary photons at detector " << i << ": " << quantity.number_of_primary_particles << std::endl;
        std::cout << "Number of secondary photons at detector " << i << ": " << quantity.number_of_secondary_particles << std::endl;

        i++;
    }

    auto materials_dose = voxel_grid.getEnergyDepositedInMaterials();
    for (auto& material_dose : materials_dose) {
        std::cout << material_dose.first << ": " << material_dose.second / N_photons << std::endl;
    }
}

int main() {
    auto data_service = setupDataService();
    auto materials = initializeMaterials(data_service);
    auto tallies = initializeTallies();

    InteractionData interaction_data(materials, data_service);
    VoxelGrid voxel_grid("data/voxels/case_2_0_degrees.nii.gz");
    Eigen::Vector3d detector_position(2, 2, 100);
    Detector detector(detector_position);
    PhysicsEngine physics_engine(voxel_grid, interaction_data, detector, tallies);



    PhotonSource source = initializeSource();

    runSimulation(source, physics_engine, 1000000);

    displayResults(voxel_grid, detector, tallies, 1000000, interaction_data);

    return 0;
}
