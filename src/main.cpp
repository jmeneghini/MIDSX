#include "source.h"
#include "physics_engine_data_service.h"
#include "probability_dist.h"
#include <helper.h>
#include <matplot/matplot.h>
#include <cmath>
#include "interpolators.h"
#include "voxel_grid.h"
#include "physics_engine.h"

namespace plt = matplot;

int main() {

    std::vector<int> elements = {55};
    PhysicsEngineDataService data_service("data/data_sources/EPDL/EPDL.db", elements);
    VoxelGrid voxel_grid(0.1, 0.1, 0.044, 0.001);
    PhysicsEngine physics_engine(voxel_grid, data_service);
    Eigen::Vector3d position(0.05, 0.05, 0);
    Eigen::Vector3d direction(0, 0, 1);
    int N_photons = 100000;
    for (int i=0; i<N_photons; i++) {
        Photon photon(position, direction, 60E3);
        physics_engine.transportPhoton(photon);
    }
    int N_exit = voxel_grid.getNumExits();
    std::cout << "N exit: " << N_exit << std::endl;

}