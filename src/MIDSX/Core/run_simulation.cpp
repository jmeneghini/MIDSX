#include "Core/run_simulation.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"

void runSimulation(PhotonSource& source, PhysicsEngine& physics_engine,
                   std::function<std::vector<std::unique_ptr<SurfaceTally>>()> surface_tally_init,
                     std::function<std::vector<std::unique_ptr<VolumeTally>>()> volume_tally_init,
                   int N_photons, double& run_time) {
    int j = 0;
    // timer
    double start_time = omp_get_wtime();
#pragma omp parallel default(none) shared(source, physics_engine, surface_tally_init, volume_tally_init, N_photons, j, std::cout)
    {
        std::vector<std::unique_ptr<SurfaceTally>> surface_tallies = surface_tally_init(); // thread private
        std::vector<std::unique_ptr<VolumeTally>> volume_tallies = volume_tally_init();
#pragma omp critical
        {
            physics_engine.addSurfaceTallies(std::move(surface_tallies));
            physics_engine.addVolumeTallies(std::move(volume_tallies));
            physics_engine.initializeVoxelData();
        }
#pragma omp barrier // wait for all threads to finish initialization
#pragma omp for
        for (int i = 0; i < N_photons; i++) {
            Photon photon = source.generatePhoton();
            physics_engine.transportPhoton(photon);
#pragma omp critical
            {
                if (i % (N_photons / 20) == 0) {
                std::cout << "Progress: " << j << "%" << std::flush << "\r";
                j += 5;
                }
            }
        }
    }
    double end_time = omp_get_wtime();
    run_time = end_time - start_time;

    physics_engine.addVoxelDataToComputationalDomain();
}
#pragma clang diagnostic pop