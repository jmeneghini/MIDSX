#ifndef VOXEL_H
#define VOXEL_H

#include <string>

struct Voxel {
    std::string material = "Air";
    double density = 0.0;
    double dose = 0.0;
};

#endif // VOXEL_H