#include "particle.h"

Eigen::Vector3d ParticleHelpers::rotateVector(const Eigen::Vector3d &vector, const double &theta, const double &phi) {
    Eigen::Vector3d rotatedVector;
    Eigen::Matrix3d rotation_matrix;
    rotation_matrix = Eigen::AngleAxisd(phi, Eigen::Vector3d::UnitZ())
                      * Eigen::AngleAxisd(theta, Eigen::Vector3d::UnitY());

    rotatedVector = rotation_matrix * vector;
    return rotatedVector;
}

Particle::Particle(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy) : position_(std::move(position)), direction_(std::move(direction)), energy_(energy) {}
