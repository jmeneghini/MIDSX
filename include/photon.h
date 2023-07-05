#ifndef PHOTON_H
#define PHOTON_H

#include <utility>

#include "vec3.h"
#include "helper.h"

class Photon {
public:
    Photon() = default;
    Photon(Eigen::Vector3d position, Eigen::Vector3d  direction, const double energy) : position(std::move(position)), direction(std::move(direction)), energy(energy) {}

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
        direction = rotateVector(direction, theta, phi);
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


    Eigen::Vector3d rotateVector(const Eigen::Vector3d& vector, const double& theta, const double& phi) {
        Eigen::Vector3d rotatedVector;
        Eigen::Matrix3d rotation_matrix;
        rotation_matrix = Eigen::AngleAxisd(phi, Eigen::Vector3d::UnitZ())
                      * Eigen::AngleAxisd(theta, Eigen::Vector3d::UnitY());

        rotatedVector = rotation_matrix * vector;
        return rotatedVector;
    }
};

#endif