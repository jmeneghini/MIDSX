#ifndef VOXELGRID_H
#define VOXELGRID_H

#include <vector>
#include "vec3.h"
#include "voxel.h"
#include <helper.h>


class VoxelGrid {
public:
    VoxelGrid() = default;
    VoxelGrid(double dimX, double dimY, double dimZ, double spacing);

    // get voxel value at (x, y, z)
    Voxel& getVoxel(const std::array<int, 3>& voxelIndex);

    // set voxel value at (x, y, z)
    void setVoxel(const std::array<int, 3>& voxelIndex, Voxel& value);

    // get spatial position of voxel at (x, y, z)
    Eigen::Vector3d getVoxelPosition(const std::array<int, 3>& voxelIndex);

    // get voxel index from spatial position
    std::array<int, 3> getVoxelIndex(const Eigen::Vector3d& position);

    double getTotalEnergyDeposited();

    // add to number of photons that have exited the voxel grid
    void addExit() {
        numExits++;
    }

    // get number of photons that have exited the voxel grid
    int getNumExits() const {
        return numExits;
    }
    

private:
    double dimX{}, dimY{}, dimZ{};
    double spacing{};
    int numX{}, numY{}, numZ{};
    int numExits = 0;
    std::vector<Voxel> voxels;

    // calculate the index of the voxel at (x, y, z)
    int voxelNumber(const std::array<int, 3>& voxelIndex) const;

    // check if (x, y, z) is within the voxel grid
    bool withinGrid(const std::array<int, 3>& voxelIndex) const;

    void handleOutOfBounds(const std::array<int, 3>& voxelIndex) const;
};

#endif // VOXELGRID_H
