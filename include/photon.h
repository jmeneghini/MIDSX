#ifndef PHOTON_H
#define PHOTON_H

#include "vec3.h"

class Photon {
public:
    Photon() {}
    Photon(const Vec3& position, const Vec3& direction, const float& energy) : position(position), direction(direction), energy(energy) {}

    Vec3 getPosition() const {return position;}
    Vec3 getDirection() const {return direction;}
    float getEnergy() const {return energy;}

    void move(const float& distance) {
        position += direction * distance;
    }

    void scatter(const Vec3& newDirection) {
        direction = newDirection;
    }

    void absorb(const float& absorption) {
        energy *= absorption;
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
    float energy;
    bool terminated = false;
};

#endif