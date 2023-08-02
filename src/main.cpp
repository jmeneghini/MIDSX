#include "source.h"
#include "interaction_data.h"
#include "probability_dist.h"
#include <helper.h>
#include <matplot/matplot.h>
#include <cmath>
#include "interpolators.h"
#include "voxel_grid.h"
#include "physics_engine.h"
#include "detector.h"

namespace plt = matplot;

int main() {
    auto data_service = std::make_shared<DataAccessObject>("data/data_sources/EPDL/EPDL.db");
    std::vector<std::shared_ptr<Material>> materials;
    materials.emplace_back(std::make_shared<Material>(Material("Water, Liquid", data_service)));
    InteractionData interaction_data(materials, data_service);
//    // plot cross sections for adipose tissue
//    auto total_cs_matrix = interaction_data.getMaterial(48).getData()->getTotalCrossSectionMatrix();
//    auto coherent_cs_matrix = interaction_data.getMaterial(48).getData()->getCoherentScatteringCrossSectionMatrix();
//    auto incoherent_cs_matrix = interaction_data.getMaterial(48).getData()->getIncoherentScatteringCrossSectionMatrix();
//    auto photoelectric_cs_matrix = interaction_data.getMaterial(48).getData()->getPhotoelectricCrossSectionMatrix();
//
//    // convert to Eigen::VectorXd
//    Eigen::VectorXd total_cs_vector = Eigen::Map<Eigen::VectorXd>(total_cs_matrix.col(1).data(), total_cs_matrix.rows());
//    Eigen::VectorXd coherent_cs_vector = Eigen::Map<Eigen::VectorXd>(coherent_cs_matrix.col(1).data(), coherent_cs_matrix.rows());
//    Eigen::VectorXd incoherent_cs_vector = Eigen::Map<Eigen::VectorXd>(incoherent_cs_matrix.col(1).data(), incoherent_cs_matrix.rows());
//    Eigen::VectorXd photoelectric_cs_vector = Eigen::Map<Eigen::VectorXd>(photoelectric_cs_matrix.col(1).data(), photoelectric_cs_matrix.rows());
//
//    // and energies
//    Eigen::VectorXd total_cs_energies_vector = Eigen::Map<Eigen::VectorXd>(total_cs_matrix.col(0).data(), total_cs_matrix.rows());
//    Eigen::VectorXd coherent_cs_energies_vector = Eigen::Map<Eigen::VectorXd>(coherent_cs_matrix.col(0).data(), coherent_cs_matrix.rows());
//    Eigen::VectorXd incoherent_cs_energies_vector = Eigen::Map<Eigen::VectorXd>(incoherent_cs_matrix.col(0).data(), incoherent_cs_matrix.rows());
//    Eigen::VectorXd photoelectric_cs_energies_vector = Eigen::Map<Eigen::VectorXd>(photoelectric_cs_matrix.col(0).data(), photoelectric_cs_matrix.rows());
//
//    // plot all on same graph
//    plt::figure();
//    plt::loglog(total_cs_energies_vector, total_cs_vector, "k")->display_name("Total");
//    plt::loglog(coherent_cs_energies_vector, coherent_cs_vector, "r")->display_name("Coherent");
//    plt::loglog(incoherent_cs_energies_vector, incoherent_cs_vector, "b")->display_name("Incoherent");
//    plt::loglog(photoelectric_cs_energies_vector, photoelectric_cs_vector, "g")->display_name("Photoelectric");
//    plt::xlabel("Energy (eV)");
//    plt::ylabel("Cross Section (cm^2/g)");
//    plt::title("Cross Sections for Water Tissue");
//    plt::legend();
//
//    plt::show();


    VoxelGrid voxel_grid(10, 10, 3.64, 0.01);
    Eigen::Vector3d detector_position(5, 5, 100);
    Detector detector(detector_position); // creates a point detector
    PhysicsEngine physics_engine(voxel_grid, interaction_data, detector);
    Eigen::Vector3d position(5, 5, 0);
    Eigen::Vector3d direction(0, 0, 1);
    int N_photons = 10000000;
    int j = 0;
    for (int i=0; i<N_photons; i++) {
        Photon photon(position, direction, 100E3);
        physics_engine.transportPhoton(photon);
        if (i % (N_photons/20) == 0) {
            std::cout << "Progress: " << j << "%" << std::flush << "\r";
            j += 5;
        }

    }
    DetectorTallies detector_tallies = detector.getTallies();
    std::cout << "Detector tallies: " << std::endl;
    std::cout << "Total number of photons detected: " << detector_tallies.total_photons_hit << std::endl;
    std::cout << "Total energy deposited: " << detector_tallies.total_energy_deposited << std::endl;
    std::cout << "Number of primary photons detected: " << detector_tallies.primary_photons_hit << std::endl;
    std::cout << "Energy deposited by primary photons: " << detector_tallies.energy_deposited_by_primary_photons << std::endl;
    std::cout << "Number of secondary photons detected: " << detector_tallies.secondary_photons_hit << std::endl;
    std::cout << "Energy deposited by secondary photons: " << detector_tallies.energy_deposited_by_secondary_photons << std::endl;
    std::cout << "Energy Deposited in Voxel Grid (ev/photon): " << voxel_grid.getTotalEnergyDeposited() / N_photons << std::endl;

}