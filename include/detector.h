#ifndef MCXRAYTRANSPORT_DETECTOR_H
#define MCXRAYTRANSPORT_DETECTOR_H

#include <Eigen/Core>
#include "particle.h"

struct DetectorTallies {
    int primary_photons_hit = 0;
    int secondary_photons_hit = 0;
    double energy_deposited_by_primary_photons = 0.0;
    double energy_deposited_by_secondary_photons = 0.0;
    int total_photons_hit = 0;
    double total_energy_deposited = 0.0;
};

class Detector {
public:
    Detector() = default;
    Detector(Eigen::Vector3d& position);

    void updateTallies(Particle& photon);

    Eigen::Vector3d getPosition() const {return position_;}

    DetectorTallies getTallies() const {return data_;}


private:
    Eigen::Vector3d position_;
    DetectorTallies data_;

    void updatePrimaryTallies(Particle& photon);
    void updateSecondaryTallies(Particle& photon);
    void updateTotalTallies(Particle& photon);


};

#endif //MCXRAYTRANSPORT_DETECTOR_H
