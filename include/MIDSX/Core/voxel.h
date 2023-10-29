#ifndef VOXEL_H
#define VOXEL_H

#include <string>

struct Voxel {
    uint8_t materialID = 0;
    double dose = 0.0;
    // For Welford's algorithm
    unsigned int count = 0;
    float mean = 0.0;
    float M2 = 0.0;
};

namespace VoxelHelpers {
    void updateWelford(Voxel& voxel, double value);
}

#endif // VOXEL_H