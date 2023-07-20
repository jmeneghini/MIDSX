#include "detector.h"

Detector::Detector(Eigen::Vector3d& position) : position_(position) {}

void Detector::updateTallies(Particle& photon) {
    if (photon.isPrimary()) {
        updatePrimaryTallies(photon);
    } else {
        updateSecondaryTallies(photon);
    }
    updateTotalTallies(photon);
}

void Detector::updatePrimaryTallies(Particle &photon) {
    data_.primary_photons_hit++;
    data_.energy_deposited_by_primary_photons += photon.getEnergy();
}

void Detector::updateSecondaryTallies(Particle &photon) {
    data_.secondary_photons_hit++;
    data_.energy_deposited_by_secondary_photons += photon.getEnergy();
}

void Detector::updateTotalTallies(Particle &photon) {
    data_.total_photons_hit++;
    data_.total_energy_deposited += photon.getEnergy();
}