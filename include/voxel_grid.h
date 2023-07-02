#ifndef VOXELGRID_H
#define VOXELGRID_H

#include <vector>
#include "vec3.h"
#include "voxel.h"
#include <helper.h>


class VoxelGrid {
public:
    VoxelGrid() {}
    VoxelGrid(double dimX, double dimY, double dimZ, double spacing) 
    : dimX(dimX), dimY(dimY), dimZ(dimZ), spacing(spacing), 
      numX(static_cast<int>(dimX/spacing)), 
      numY(static_cast<int>(dimY/spacing)), 
      numZ(static_cast<int>(dimZ/spacing)), 
      voxels(numX * numY * numZ, Voxel()) {} // initialize voxels to default values

    // get voxel value at (x, y, z)
    Voxel getVoxel(std::array<int, 3> voxelIndex) {
        handleOutOfBounds(voxelIndex);
        return voxels[voxelNumber(voxelIndex)];

    }

    // set voxel value at (x, y, z)
    void setVoxel(std::array<int, 3> voxelIndex, Voxel& value) {
        handleOutOfBounds(voxelIndex);
        voxels[voxelNumber(voxelIndex)] = value;
    }

    // get spatial position of voxel at (x, y, z)
    Vec3 getVoxelPosition(std::array<int, 3> voxelIndex) {
        handleOutOfBounds(voxelIndex);
        return Vec3(voxelIndex[0]*spacing, voxelIndex[1]*spacing, voxelIndex[2]*spacing);
    }

    // get voxel index from spatial position
    std::array<int, 3> getVoxelIndex(const Vec3& position) {
        int x = static_cast<int>(position.x()/spacing);
        int y = static_cast<int>(position.y()/spacing);
        int z = static_cast<int>(position.z()/spacing);
        std::array<int, 3> voxelIndex = {x, y, z};
        handleOutOfBounds(voxelIndex);
        return voxelIndex;
    }
    

private:
    double dimX, dimY, dimZ;
    double spacing;
    int numX, numY, numZ;
    std::vector<Voxel> voxels;

    // calculate the index of the voxel at (x, y, z)
    int voxelNumber(std::array<int, 3> voxelIndex) const {
        return voxelIndex[0] + voxelIndex[1]*numX + voxelIndex[2]*numX*numY;
    }

    // check if (x, y, z) is within the voxel grid
    bool withinGrid(std::array<int, 3> voxelIndex) const {
        return (0 <= voxelIndex[0] && voxelIndex[0] < numX) && (0 <= voxelIndex[1] && voxelIndex[1] < numY) && (0 <= voxelIndex[2] && voxelIndex[2] < numZ);
    }

    void handleOutOfBounds(std::array<int, 3> voxelIndex) const {
        if (!withinGrid(voxelIndex)) {
            throw std::out_of_range("VoxelGrid::handleOutOfBounds: voxel index out of range");
        }
    }
};

#endif // VOXELGRID_H

// USE XCOM FOR CROSS SECTIONS