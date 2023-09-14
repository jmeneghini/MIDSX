#include "Core/particle.h"

namespace {
    const double EPSILON = 1E-9;
}

Eigen::Vector3d ParticleHelpers::rotateDirection(const Eigen::Vector3d &vector, const double &theta, const double &phi) {
    // check if pointing in z direction
    if (1 - abs(vector.z()) < EPSILON) {
        double new_dir_x = vector.z() * sin(theta) * cos(phi);
        double new_dir_y = vector.z() * sin(theta) * sin(phi);
        double new_dir_z = vector.z() * cos(theta);
        return {new_dir_x, new_dir_y, new_dir_z};
    }
    double new_dir_x = vector.x() * cos(theta) - sin(theta)/(sqrt(1 - vector.z()*vector.z())) * (vector.x()*vector.z()*cos(phi) + vector.y()*sin(phi));
    double new_dir_y = vector.y() * cos(theta) - sin(theta)/(sqrt(1 - vector.z()*vector.z())) * (vector.y()*vector.z()*cos(phi) - vector.x()*sin(phi));
    double new_dir_z = vector.z() * cos(theta) + sqrt(1 - vector.z()*vector.z()) * sin(theta) * cos(phi);
    return {new_dir_x, new_dir_y, new_dir_z};
    // TODO: sample cos(phi) and sin(phi) directly instead. This will be more efficient.
}

Particle::Particle(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy) : position_(std::move(position)), direction_(std::move(direction)), energy_(energy) {}
