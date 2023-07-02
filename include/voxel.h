#ifndef VOXEL_H
#define VOXEL_H

#include <string>
#include "vec3.h"

struct Voxel {
    int element;
    double density;
    double dose = 0.0;
};

#endif // VOXEL_H