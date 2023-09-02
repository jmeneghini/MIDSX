#ifndef MCXRAYTRANSPORT_PARTICLE_H
#define MCXRAYTRANSPORT_PARTICLE_H

#include <Eigen/Core>
#include <Eigen/Dense>

namespace ParticleHelpers{
    Eigen::Vector3d rotateDirection(const Eigen::Vector3d& vector, const double& theta, const double& phi);
}


class Particle {
public:
    Particle() = default;
    Particle(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy);
    Eigen::Vector3d getPosition() const {return position_;}
    Eigen::Vector3d getDirection() const {return direction_;}
    double getEnergy() const {return energy_;}

    void move(const double& distance) {
        position_ += distance * direction_;
    }

    void setDirection(const Eigen::Vector3d& newDirection) {
        direction_ = newDirection;
    }

    void rotate(const double& theta, const double& phi) {
        direction_ = ParticleHelpers::rotateDirection(direction_, theta, phi);
    }

    void setEnergy(const double& newEnergy) {
        energy_ = newEnergy;
    }

    bool isTerminated() const {
        return terminated_;
    }

    void terminate() {
        terminated_ = true;
    }

    bool isPrimary() const {
        return isPrimary_;
    }

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