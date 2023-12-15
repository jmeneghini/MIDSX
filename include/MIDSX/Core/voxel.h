#ifndef VOXEL_H
#define VOXEL_H

#include <string>

/**
 * @brief Struct which represents a voxel.
 *
 * Stores the material ID and dose for a voxel.
 */
struct Voxel {
    uint8_t materialID = 0;
    double dose = 0.0;
    // For Welford's algorithm
    unsigned int count = 0;
    float mean = 0.0;
    float M2 = 0.0;
};

namespace VoxelHelpers {
    /**
     * @brief Uses Welford's algorithm to update the mean and variance of voxel does and count.
     * @param voxel
     * @param value
     */
    void updateWelford(Voxel& voxel, double value);
}

#endif // VOXEL_H