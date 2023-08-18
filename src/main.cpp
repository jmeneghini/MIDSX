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
    materials.emplace_back(std::make_shared<Material>("Al", data_service));
    materials.emplace_back(std::make_shared<Material>("Air, Dry (near sea level)", data_service));
    return materials;
}

std::vector<std::shared_ptr<Tally>> initializeTallies() {
    std::vector<std::shared_ptr<Tally>> tallies;
    tallies.emplace_back(std::make_shared<DiscSurfaceTally>(
            Eigen::Vector3d(2, 2, 3.022),
            Eigen::Vector3d(0, 0, 1),
            0.001,
            QuantityContainerFactory::AllQuantities()));

    tallies.emplace_back(std::make_shared<DiscSurfaceTally>(
            Eigen::Vector3d(2, 2, 0),
            Eigen::Vector3d(0, 0, 1),
            0.001,
            QuantityContainerFactory::AllQuantities()));

    return tallies;
}

Eigen::MatrixXd processEnergySpectrum() {
    Eigen::MatrixXd energy_spectrum = SourceHelpers::readCSV("data/source_distributions/RQRM3_MO_MO_30kVp_E_spectrum.csv");
    energy_spectrum.col(0) = energy_spectrum.col(0).array() * 1000;  // Convert from keV to eV
    return energy_spectrum;
}

void runSimulation(PhotonSource& source, PhysicsEngine& physics_engine, int N_photons) {
    int j = 0;
//    std::cout << "Maximum threads that can be used: " << omp_get_num_procs() << std::endl;
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

void displayResults(VoxelGrid& voxel_grid, const Detector& detector, const std::vector<std::shared_ptr<Tally>>& tallies, int N_photons, InteractionData& interaction_data, const Eigen::MatrixXd& energy_spectrum) {
    auto quantity_container_2 = tallies[0]->getQuantityContainer();
    quantity_container_2->processMeasurements();
    auto quantity_2 = quantity_container_2->getTallyData();
    auto primary_air_kerma_2 = quantity_container_2->getPrimaryAirKerma(interaction_data, 3, energy_spectrum.col(0));


    auto quantity_container_1 = tallies[1]->getQuantityContainer();
    quantity_container_1->processMeasurements();
    auto quantity_1 = quantity_container_1->getTallyData();
    auto primary_air_kerma_1 = quantity_container_1->getPrimaryAirKerma(interaction_data, 3, energy_spectrum.col(0));

    std::cout << "Air kerma at detector 2: " << primary_air_kerma_2 << std::endl;
    std::cout << "Air kerma at detector 1: " << primary_air_kerma_1 << std::endl;
    std::cout << "Air kerma ratio: " << primary_air_kerma_2 / primary_air_kerma_1 << std::endl;
    std::cout << "Number of primary photons at detector 2: " << quantity_2.number_of_primary_particles << std::endl;
    std::cout << "Number of primary photons at detector 1: " << quantity_1.number_of_primary_particles << std::endl;
    std::cout << "Number of secondary photons at detector 2: " << quantity_2.number_of_secondary_particles << std::endl;
    std::cout << "Number of secondary photons at detector 1: " << quantity_1.number_of_secondary_particles << std::endl;
    std::cout << "Number of total photons at detector 2: " << quantity_2.number_of_particles << std::endl;
    std::cout << "Number of total photons at detector 1: " << quantity_1.number_of_particles << std::endl;
    std::cout << "length of primary cosine: " << quantity_2.primary_entrance_cosines.size() << std::endl;
    std::cout << "length of cosine: " << quantity_1.primary_entrance_cosines.size() << std::endl;

    DetectorTallies detector_tallies = detector.getTallies();
    std::cout << "Detector tallies: " << std::endl;
    std::cout << "Total number of photons detected: " << detector_tallies.total_photons_hit << std::endl;
    std::cout << "Total energy deposited: " << detector_tallies.total_energy_deposited << std::endl;
    std::cout << "Number of primary photons detected: " << detector_tallies.primary_photons_hit << std::endl;
    std::cout << "Energy deposited by primary photons: " << detector_tallies.energy_deposited_by_primary_photons << std::endl;
    std::cout << "Number of secondary photons detected: " << detector_tallies.secondary_photons_hit << std::endl;
    std::cout << "Energy deposited by secondary photons: " << detector_tallies.energy_deposited_by_secondary_photons << std::endl;
    std::cout << "Energy Deposited in Voxel Grid (ev/photon): " << voxel_grid.getTotalEnergyDeposited() / N_photons << std::endl;
    std::cout << "Energy Deposited in Voxel Grid per material (ev/photon): " << std::endl;
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
    VoxelGrid voxel_grid("data/voxels/al_qvl_100keV.nii.gz");
    Eigen::Vector3d detector_position(2, 2, 100);
    Detector detector(detector_position);
    PhysicsEngine physics_engine(voxel_grid, interaction_data, detector, tallies);
    auto energy_spectrum = processEnergySpectrum();

//    PolyenergeticSpectrum poly_spectrum(energy_spectrum);
    MonoenergeticSpectrum mono_spectrum(100E3);
    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<MonoenergeticSpectrum>(mono_spectrum);

    Eigen::Vector3d voxel_origin_to_max = voxel_grid.getDimSpace();
    std::unique_ptr<Directionality> directionality = std::make_unique<BeamDirectionality>(BeamDirectionality(detector_position));

    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(PointGeometry(Eigen::Vector3d(2, 2, 0)));

    PhotonSource source(std::move(spectrum), std::move(directionality), std::move(geometry));

    runSimulation(source, physics_engine, 10000000);

    displayResults(voxel_grid, detector, tallies, 10000000, interaction_data, energy_spectrum);

    return 0;
}
