#ifndef MCXRAYTRANSPORT_PARTICLE_H
#define MCXRAYTRANSPORT_PARTICLE_H

#include <Eigen/Core>
#include <Eigen/Dense>

namespace ParticleHelpers{

    /**
     * @brief Rotates a vector by theta and phi, which respect to the initial vector.
     *
     * @param vector The vector to be rotated.
     * @param theta The angle to rotate the vector by.
     * @param phi The angle to rotate the vector by.
     * @return The rotated vector.
     */
    Eigen::Vector3d rotateDirection(const Eigen::Vector3d& vector, const double& theta, const double& phi);


    /**
     * @brief Returns a vector perpendicular to the given vector.
     *
     * The returned vector is not normalized and could be any vector perpendicular to the given vector.
     *
     * @param vector The vector to find a perpendicular vector to.
     * @return The perpendicular vector.
     */
    Eigen::Vector3d getPerpendicularVector(const Eigen::Vector3d& vector);
}

/**
 * @brief Class which represents a particle.
 *
 * This class contains the properties of a particle.
 */
class Particle {
public:
    Particle() = default;
    /**
     * @brief Constructor for the Particle class.
     *
     * @param position The initial position of the particle.
     * @param direction The initial direction of the particle.
     * @param energy The initial energy of the particle.
     */
    Particle(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy);

    /**
     * @brief Returns the position of the particle.
     *
     * @return The position of the particle.
     */
    Eigen::Vector3d getPosition() const {return position_;}

    /**
     * @brief Returns the direction of the particle.
     *
     * @return The direction of the particle.
     */
    Eigen::Vector3d getDirection() const {return direction_;}

    /**
     * @brief Returns the energy of the particle.
     *
     * @return The energy of the particle.
     */
    double getEnergy() const {return energy_;}

    /**
     * @brief Moves the particle by the given distance in the direction of the particle.
     *
     * @param distance The distance to move the particle by.
     */
    void move(const double& distance) {
        position_ += distance * direction_;
    }

    /**
     * @brief Sets the direction of the particle.
     *
     * @param newDirection The new direction of the particle.
     */
    void setDirection(const Eigen::Vector3d& newDirection) {
        direction_ = newDirection;
    }

    /**
     * @brief Rotates the particle by the given angles with respect to the initial direction of the particle, represented by the z axis.
     *
     * The z
     *
     * @param theta The polar angle to rotate the particle by.
     * @param phi The azimuthal to rotate the particle by.
     */
    void rotate(const double& theta, const double& phi) {
        direction_ = ParticleHelpers::rotateDirection(direction_, theta, phi);
    }

    /**
     * @brief Sets the energy of the particle.
     *
     * @param newEnergy The new energy of the particle.
     */
    void setEnergy(const double& newEnergy) {
        energy_ = newEnergy;
    }

    /**
     * @brief Returns whether or not the particle has terminated.
     *
     * @return Whether or not the particle has terminated.
     */
    bool isTerminated() const {
        return terminated_;
    }

    /**
     * @brief Terminates the particle.
     */
    void terminate() {
        terminated_ = true;
    }

    /**
     * @brief Returns whether or not the particle is primary.
     *
     * @return Whether or not the particle is primary.
     */
    bool isPrimary() const {
        return isPrimary_;
    }

    /**
     * @brief Sets whether or not the particle is primary.
     *
     * @param primary_status Whether or not the particle is primary.
     */
    void setPrimary(bool primary_status) {
        isPrimary_ = primary_status;
    }
private:
    Eigen::Vector3d position_;
    Eigen::Vector3d direction_;
    double energy_{};
    bool terminated_ = false;
    bool isPrimary_ = true;
};

#endif //MCXRAYTRANSPORT_PARTICLE_H