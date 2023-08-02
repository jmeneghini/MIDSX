#include "source.h"
#include "interaction_data.h"
#include "probability_dist.h"
#include <matplot/matplot.h>
#include "voxel_grid.h"
#include "physics_engine.h"
#include "detector.h"

namespace plt = matplot;

int main() {
    auto data_service = std::make_shared<DataAccessObject>("data/data_sources/EPDL/EPDL.db");
    std::vector<std::shared_ptr<Material>> materials;
    materials.emplace_back(std::make_shared<Material>(Material("Al", data_service)));
    InteractionData interaction_data(materials, data_service);
    VoxelGrid voxel_grid(10, 10, 0.69, 0.01);
    Eigen::Vector3d detector_position(5, 5, 100);
    Detector detector(detector_position); // creates a point detector
    PhysicsEngine physics_engine(voxel_grid, interaction_data, detector);
    Eigen::Vector3d position(5, 5, 0);
    Eigen::Vector3d direction(0, 0, 1);
    int N_photons = 10000000;
    int j = 0;
    for (int i=0; i<N_photons; i++) {
        Photon photon(position, direction, 50E3);
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