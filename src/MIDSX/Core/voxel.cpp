#include "Core/voxel.h"

void VoxelHelpers::updateWelford(Voxel &voxel, double value) {
    voxel.dose += value;
    // Welford's Online Algorithm
    voxel.count++;
    double delta = value - voxel.mean;
    voxel.mean += delta / voxel.count;
    double delta2 = value - voxel.mean;
    voxel.M2 += delta * delta2;
}
