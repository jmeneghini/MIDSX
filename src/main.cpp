#include "source.h"
#include "physics_engine_data_service.h"
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
    std::vector<int> elements = {13};
    PhysicsEngineDataService data_service("data/data_sources/EPDL/EPDL.db", elements);
    VoxelGrid voxel_grid(10, 10, 1.49, 0.01);
    Eigen::Vector3d detector_position(5, 5, 100);
    Detector detector(detector_position); // creates a point detector
    PhysicsEngine physics_engine(voxel_grid, data_service, detector);
    Eigen::Vector3d position(5, 5, 0);
    Eigen::Vector3d direction(0, 0, 1);
    int N_photons = 1000000;
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

}