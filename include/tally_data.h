#ifndef MCXRAYTRANSPORT_TALLY_DATA_H
#define MCXRAYTRANSPORT_TALLY_DATA_H

#include "photon.h"

struct TempTallyData {
    double energy_deposited = 0.0;
    double free_path = 0.0;
    bool isInteract = false;
    double entrance_cosine = 0.0;
    Photon initial_photon;
};

struct TallyData {
    Eigen::Vector<double, Eigen::Dynamic> energy_deposition;
    Eigen::Vector<double, Eigen::Dynamic> incident_energy;
    Eigen::Vector<double, Eigen::Dynamic> primary_incident_energy;
    int number_of_particles = 0;
    int number_of_primary_particles = 0;
    int number_of_secondary_particles = 0;
    Eigen::Vector<double, Eigen::Dynamic> entrance_cosines;
    Eigen::Vector<double, Eigen::Dynamic> primary_entrance_cosines;
    int number_of_interactions = 0;
    double area = 0.0;
};

#endif //MCXRAYTRANSPORT_TALLY_DATA_H
