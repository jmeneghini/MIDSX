#ifndef VOXELGRID_H
#define VOXELGRID_H

#include <vector>
#include "vec3.h"
#include "voxel.h"


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
    Voxel getVoxel(int x, int y, int z) {
        withinGrid(x, y, z);
        return voxels[index(x, y, z)];
    }

    // set voxel value at (x, y, z)
    void setVoxel(int x, int y, int z, Voxel& value) {
        withinGrid(x, y, z);
        voxels[index(x, y, z)] = value;
    }

    // get spatial position of voxel at (x, y, z)
    Vec3 getVoxelPosition(int x, int y, int z) {
        withinGrid(x, y, z);
        return Vec3(x*spacing, y*spacing, z*spacing);
    }

    // get voxel index from spatial position
    int getVoxelIndex(const Vec3& position) {
        int x = static_cast<int>(position.x()/spacing);
        int y = static_cast<int>(position.y()/spacing);
        int z = static_cast<int>(position.z()/spacing);
        withinGrid(x, y, z);
        return index(x, y, z);
    }
    

private:
    double dimX, dimY, dimZ;
    double spacing;
    int numX, numY, numZ;
    std::vector<Voxel> voxels;

    // calculate the index of the voxel at (x, y, z)
    int index(int x, int y, int z) const {
        return x + y*numX + z*numX*numY;
    }

    // check if (x, y, z) is within the voxel grid
    void withinGrid(int x, int y, int z) const {
        if (x < 0 || x >= numX || y < 0 || y >= numY || z < 0 || z >= numZ) {
            throw std::out_of_range("VoxelGrid::withinGrid: (x, y, z) is not within the voxel grid");
        }
    }
};

#endif // VOXELGRID_H
