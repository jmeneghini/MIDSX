#include "source.h"
#include "physics_engine_data_service.h"
#include <probability_dist.h>
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
    VoxelGrid voxel_grid(1E-2, 1E-2, 2E-3, 1E-2);
    PhysicsEngine physics_engine(voxel_grid, data_service);
    Eigen::Vector3d position(0.5e-3, 0.5E-3, 0);
    Eigen::Vector3d direction(0, 0, 1);
    int N_photons = 10;
    for (int i=0; i<N_photons; i++) {
        Photon photon(position, direction, 30E4);
        physics_engine.transportPhoton(photon);
    }
    int N_absorbed = static_cast<int>(voxel_grid.getTotalDose()/30E3);
    std::cout << "N absorbed: " << N_absorbed << std::endl;

}