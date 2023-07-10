#include "voxel_grid.h"

VoxelGrid::VoxelGrid(double dimX, double dimY, double dimZ, double spacing): dimX(dimX), dimY(dimY), dimZ(dimZ), spacing(spacing),
                                                                             numX(static_cast<int>(dimX/spacing)),
                                                                             numY(static_cast<int>(dimY/spacing)),
                                                                             numZ(static_cast<int>(dimZ/spacing)),
                                                                             voxels(numX * numY * numZ, Voxel()) {};
// initialize voxels to default values

// get voxel value at (x, y, z)
Voxel& VoxelGrid::getVoxel(const std::array<int, 3>& voxelIndex) {
    handleOutOfBounds(voxelIndex);
    return voxels[voxelNumber(voxelIndex)];
}

void VoxelGrid::setVoxel(const std::array<int, 3> &voxelIndex, Voxel &value) {
    handleOutOfBounds(voxelIndex);
    voxels[voxelNumber(voxelIndex)] = value;
}

Eigen::Vector3d VoxelGrid::getVoxelPosition(const std::array<int, 3>& voxelIndex) {
    handleOutOfBounds(voxelIndex);
    return Eigen::Vector3d {voxelIndex[0]*spacing, voxelIndex[1]*spacing, voxelIndex[2]*spacing};
}

std::array<int, 3> VoxelGrid::getVoxelIndex(const Eigen::Vector3d& position) {
    int x = static_cast<int>(position.x()/spacing);
    int y = static_cast<int>(position.y()/spacing);
    int z = static_cast<int>(position.z()/spacing);
    std::array<int, 3> voxelIndex = {x, y, z};
    handleOutOfBounds(voxelIndex);
    return voxelIndex;
}

double VoxelGrid::getTotalEnergyDeposited() {
    double totalDose = 0.0;
    for (auto& voxel : voxels) {
        totalDose += voxel.dose;
    }
    return totalDose;
}

int VoxelGrid::voxelNumber(const std::array<int, 3>& voxelIndex) const {
    return voxelIndex[0] + voxelIndex[1]*numX + voxelIndex[2]*numX*numY;
}

bool VoxelGrid::withinGrid(const std::array<int, 3>& voxelIndex) const {
    bool is_in = (0 <= voxelIndex[0] && voxelIndex[0] < numX) && (0 <= voxelIndex[1] && voxelIndex[1] < numY) && (0 <= voxelIndex[2] && voxelIndex[2] < numZ);
    return is_in;
}

void VoxelGrid::handleOutOfBounds(const std::array<int, 3>& voxelIndex) const {
    if (!withinGrid(voxelIndex)) {
        throw std::out_of_range("VoxelGrid::handleOutOfBounds: voxel index out of range");
    }
}





