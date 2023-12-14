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
 * @param N_photons The number of photons to simulate.
 */
void runSimulation(PhotonSource& source, PhysicsEngine& physics_engine, int num_photons);

#endif //HVL_RUN_SIMULATION_H
