#include "Core/run_simulation.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
/**
 * @brief Runs a photon transport simulation.
 *
 * @param source The source of photons.
 * @param physics_engine The physics engine.
 * @param surface_tally_init A function which returns a vector of surface tallies.
 * @param volume_tally_init A function which returns a vector of volume tallies.
 * @param N_photons The number of photons to simulate.
 */
void runSimulation(PhotonSource& source, PhysicsEngine& physics_engine,
                   std::function<std::vector<std::unique_ptr<SurfaceTally>>()> surface_tally_init,
                     std::function<std::vector<std::unique_ptr<VolumeTally>>()> volume_tally_init,
                   int N_photons) {
    int j = 0;
#pragma omp parallel default(none) shared(source, physics_engine, surface_tally_init, volume_tally_init, N_photons, j, std::cout)
    {
        std::vector<std::unique_ptr<SurfaceTally>> surface_tallies = surface_tally_init(); // thread private
        std::vector<std::unique_ptr<VolumeTally>> volume_tallies = volume_tally_init();
#pragma omp critical
        {
            physics_engine.addSurfaceTallies(std::move(surface_tallies));
            physics_engine.addVolumeTallies(std::move(volume_tallies));
        }
#pragma omp for
        for (int i = 0; i < N_photons; i++) {
            Photon photon = source.generatePhoton();
            physics_engine.transportPhoton(photon);
            if (i % (N_photons / 20) == 0) {
                std::cout << "Progress: " << j << "%" << std::flush << "\r";
                j += 5;
            }
        }
    }
}
#pragma clang diagnostic pop