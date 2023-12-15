#ifndef MCXRAYTRANSPORT_TALLY_DATA_H
#define MCXRAYTRANSPORT_TALLY_DATA_H

#include "photon.h"
#include "voxel.h"

/**
 * @brief Struct which represents the temporary data for a voxel.
 *
 * Solely exists for parallelizing photon transport.
 */
struct TempVoxelData {
    Voxel& voxel;
    double energy_deposited = 0.0;

    explicit TempVoxelData(Voxel& voxel) : voxel(voxel) {}
};

/**
 * @brief Struct which represents the temporary data for a surface tally.
 *
 * Stores data for a photon undergoing a single step.
 */
struct TempSurfaceTallyData {
    double free_path = 0.0;
    bool isInteract = false;
    double entrance_cosine = 0.0;
    bool already_counted = false;
    Photon initial_photon;
};

/**
 * @brief Struct which represents the temporary data for a volume tally.
 *
 * Stores data for a photon undergoing a single step.
 */
struct TempVolumeTallyData {
    double energy_deposited = 0.0;
    double free_path = 0.0;
    bool isInteract = false;
    bool already_counted = false;
    Photon initial_photon;
    Photon final_photon;
};

/**
 * @brief Struct which stores data for a volume tally.
 *
 * Stores data for a volume tally.
 */
struct VolumeTallyData {
    Eigen::Vector<double, Eigen::Dynamic> energy_deposition;
    Eigen::Vector<double, Eigen::Dynamic> incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> primary_incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> secondary_incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> single_coherent_incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> single_incoherent_incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> multiple_scatter_incident_energy;
    int number_of_particles = 0;
    int number_of_primary_particles = 0;
    int number_of_secondary_particles = 0;
    Eigen::Vector<double, Eigen::Dynamic> entrance_cosines;
    Eigen::Vector<double, Eigen::Dynamic> primary_entrance_cosines;
    int number_of_interactions = 0;
    double area = 0.0;
};

/**
 * @brief Struct which stores data for a surface tally.
 *
 * Stores data for a surface tally.
 */
struct SurfaceTallyData {
    Eigen::Vector<double, Eigen::Dynamic> energy_deposition;
    Eigen::Vector<double, Eigen::Dynamic> incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> primary_incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> secondary_incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> single_coherent_incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> single_incoherent_incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> multiple_scatter_incident_energy;
    int number_of_particles = 0;
    int number_of_primary_particles = 0;
    int number_of_secondary_particles = 0;
    Eigen::Vector<double, Eigen::Dynamic> entrance_cosines;
    Eigen::Vector<double, Eigen::Dynamic> primary_entrance_cosines;
    int number_of_interactions = 0;
    double area = 0.0;
};

#endif //MCXRAYTRANSPORT_TALLY_DATA_H
