#include "Core/particle.h"

namespace {
    const double EPSILON = 1E-9;
}

Eigen::Vector3d ParticleHelpers::rotateDirection(const Eigen::Vector3d &vector, const double &theta, const double &phi) {
    // First find a perpendicular vector to the given vector.
    // Must be normalized for use with quaternion rotation
    Eigen::Vector3d perp_vector = getPerpendicularVector(vector).normalized();

    // Using quaternion rotation, rotate the vector about the perpendicular vector by theta (polar angle)
    // Eigen has some nice helper functions for this
//    auto rotation = Eigen::AngleAxisd(theta, perp_vector);
//    Eigen::Vector3d rotated_vector = rotation * perp_vector;
//
//    // Now rotate about the direction vector by phi (azimuthal angle)
//    rotation = Eigen::AngleAxisd(phi, vector);
//    rotated_vector = rotation * rotated_vector;

    Eigen::Quaterniond q_theta(cos(theta/2), perp_vector.x()*sin(theta/2), perp_vector.y()*sin(theta/2), perp_vector.z()*sin(theta/2));
    Eigen::Quaterniond q_phi(cos(phi/2), vector.x()*sin(phi/2), vector.y()*sin(phi/2), vector.z()*sin(phi/2));
    Eigen::Quaterniond q = q_phi * q_theta;
    Eigen::Quaterniond v(0, vector.x(), vector.y(), vector.z());

    Eigen::Quaterniond rotated_vector = q * v * q.inverse();


    return {rotated_vector.x(), rotated_vector.y(), rotated_vector.z()};
}

Eigen::Vector3d ParticleHelpers::getPerpendicularVector(const Eigen::Vector3d &vector) {
    // Find the smallest component
    // Will be used to find the perp vector
    if (std::abs(vector.x()) <= std::abs(vector.y()) && std::abs(vector.x()) <= std::abs(vector.z())) {
        // x is the smallest component
        return Eigen::Vector3d::UnitX().cross(vector);
    } else if (std::abs(vector.y()) <= std::abs(vector.z())) {
        // y is the smallest component
        return Eigen::Vector3d::UnitY().cross(vector);
    } else {
        // z is the smallest component
        return Eigen::Vector3d::UnitZ().cross(vector);
    }
}

Particle::Particle(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy) : position_(std::move(position)), direction_(std::move(direction)), energy_(energy) {}
