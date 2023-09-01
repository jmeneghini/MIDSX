#ifndef VOXELGRID_H
#define VOXELGRID_H

#include <vector>
#include "voxel.h"
#include <Eigen/Dense>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <unordered_map>
#include <iostream>
#include <utility>

namespace py = pybind11;

namespace VoxelGridHelpers {
    template <typename T, std::size_t N>
    Eigen::Vector<T, N> pyTupleToEigenVector(const py::tuple &tuple);
    double covertUnitNameToCm(const std::string& unitName);
    int dimensionStringToIndex(const std::string& dimension);
}

class VoxelGrid {
public:
    VoxelGrid() = default;
    explicit VoxelGrid(std::string  filename, bool is_python_environment = false);

    // get voxel value at (i, j, k)
    Voxel& getVoxel(const Eigen::Vector3i& voxel_index);

    // set voxel value at (i, j, k)
    void setVoxel(const Eigen::Vector3i& voxel_index, Voxel& value);

    // get spatial position of voxel at (x, y, z)
    Eigen::Vector3d getVoxelPosition(const Eigen::Vector3i& voxel_index);

    // check if (x, y, z) is within the voxel grid
    bool withinGrid(const Eigen::Vector3d& position) const;

    // get voxel index from spatial position
    Eigen::Vector3i getVoxelIndex(const Eigen::Vector3d& position) const;

    double getTotalEnergyDeposited();

    std::unordered_map <int, double> getEnergyDepositedInMaterials();

    // add to number of photons that have exited the voxel grid
    void addExit() {
        numExits_++;
    }

    // get number of photons that have exited the voxel grid
    int getNumExits() const {
        return numExits_;
    }


    Eigen::Vector3d getDimSpace() const {
        return dim_space_;
    }

private:
    Eigen::Vector3i dim_vox_;
    Eigen::Vector3d spacing_; // in cm
    Eigen::Vector3d dim_space_; // in cm
    int numOfVoxels_ = 0;
    int numExits_ = 0;
    std::vector<Voxel> voxels_;
    std::string filename_;
    double units_ = 1.0; // in cm
    bool is_python_environment_;

    // initialize voxels according to the nifti file
    void initializeVoxels();

    // calculate the index of the voxel at (i, j, k)
    int voxelNumber(const Eigen::Vector3i& voxel_index) const;

    void handleOutOfBounds(const Eigen::Vector3d& position) const;

    void setVoxelProperties(const py::array_t<uint8_t>& array, const py::object &header);

    static py::array_t<uint8_t> getNumPyArrayFromImg(const py::object& img);
    void setDimVox(const py::array_t<uint8_t>& array);
    void setUnits(const py::object& header);
    void setSpacing(const py::object& header);
    void setNumOfVoxels();
    void setDimSpace();
    void setVoxelMaterialIDs(const py::array_t<uint8_t>& array);
};

#endif // VOXELGRID_H
