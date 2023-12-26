/**
 * @file run_simulation.h
 * @brief Contains the runSimulation function.
 */

#ifndef HVL_RUN_SIMULATION_H
#define HVL_RUN_SIMULATION_H

#include "source.h"
#include "physics_engine.h"

/**
 * @brief Helper function which runs the particle transport simulation.
 *
 * Uses OpenMP to parallelize the simulation. Splits the number of photons among the threads.
 *
 * @param source The photon source.
 * @param physics_engine The physics engine.
 * @param surface_tally_init A function which returns a vector of unique pointers to surface tallies.
 * @param volume_tally_init A function which returns a vector of unique pointers to volume tallies.
 * @param N_photons The number of photons to simulate.
 * @param run_time A reference to a double which stores the run time of the simulation in seconds.
 */
void runSimulation(PhotonSource& source, PhysicsEngine& physics_engine,
                   std::function<std::vector<std::unique_ptr<SurfaceTally>>()> surface_tally_init,
                   std::function<std::vector<std::unique_ptr<VolumeTally>>()> volume_tally_init,
                   int N_photons, double& run_time = *(new double));

#endif //HVL_RUN_SIMULATION_H
