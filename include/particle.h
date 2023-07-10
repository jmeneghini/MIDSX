#ifndef MCXRAYTRANSPORT_PARTICLE_H
#define MCXRAYTRANSPORT_PARTICLE_H

#include <Eigen/Core>
#include <Eigen/Dense>

namespace ParticleHelpers{
    Eigen::Vector3d rotateVector(const Eigen::Vector3d& vector, const double& theta, const double& phi);
}


class Particle {
public:
    Particle() = default;
    Particle(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy);
    Eigen::Vector3d getPosition() const {return position;}
    Eigen::Vector3d getDirection() const {return direction;}
    double getEnergy() const {return energy;}

    void move(const double& distance) {
        position += distance * direction;
    }

    void setDirection(const Eigen::Vector3d& newDirection) {
        direction = newDirection;
    }

    void rotate(const double& theta, const double& phi) {
        direction = ParticleHelpers::rotateVector(direction, theta, phi);
    }

    void setEnergy(const double& newEnergy) {
        energy = newEnergy;
    }

    bool isTerminated() const {
        return terminated;
    }

    void terminate() {
        terminated = true;
    }

private:
    Eigen::Vector3d position;
    Eigen::Vector3d direction;
    double energy{};
    bool terminated = false;
};

#endif //MCXRAYTRANSPORT_PARTICLE_H