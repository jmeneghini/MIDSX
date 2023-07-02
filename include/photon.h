#ifndef PHOTON_H
#define PHOTON_H

#include "vec3.h"

class Photon {
public:
    Photon() {}
    Photon(const Vec3& position, const Vec3& direction, const double& energy) : position(position), direction(direction), energy(energy) {}

    Vec3 getPosition() const {return position;}
    Vec3 getDirection() const {return direction;}
    double getEnergy() const {return energy;}

    void move(const double& distance) {
        position += distance * direction;
    }

    void setDirection(const Vec3& newDirection) {
        direction = newDirection;
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
    Vec3 position;
    Vec3 direction;
    double energy;
    bool terminated = false;
};

#endif