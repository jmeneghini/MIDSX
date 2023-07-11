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

    std::vector<int> elements = {13};
    PhysicsEngineDataService data_service("data/data_sources/EPDL/EPDL.db", elements);
    VoxelGrid voxel_grid(10, 10, 0.01, 0.01);
    PhysicsEngine physics_engine(voxel_grid, data_service);
    Eigen::Vector3d position(5, 5, 0);
    Eigen::Vector3d direction(0, 0, 1);
    int N_photons = 1000000;
    for (int i=0; i<N_photons; i++) {
        Photon photon(position, direction, 20E3);
        physics_engine.transportPhoton(photon);
    }
    int N_exit = voxel_grid.getNumExits();
    std::cout << "N exit: " << N_exit << std::endl;

}