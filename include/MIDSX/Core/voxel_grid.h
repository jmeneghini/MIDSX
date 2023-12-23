#ifndef VOXELGRID_H
#define VOXELGRID_H

#include <vector>
#include "voxel.h"
#include "interaction_data.h"
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

/**
 * @brief Class which represents a voxel grid.
 *
 * Stores the voxels and the properties of the voxel grid.
 */
class VoxelGrid {
public:
    VoxelGrid() = default;

    /**
     * @brief Constructor for the VoxelGrid class.
     *
     * @param dim_vox The dimensions of the voxel grid in voxels.
     * @param is_python_environment Whether or not the VoxelGrid is being constructed in a Python environment. Only True for python wrapper only.
     */
    explicit VoxelGrid(std::string  filename, bool is_python_environment = false);

    /**
     * @brief Gets the voxel at (i, j, k).
     *
     * @param voxel_index
     * @return
     */
    Voxel& getVoxel(const Eigen::Vector3i& voxel_index);

    /**
     * @brief Sets the voxel at (i, j, k) to specified voxel
     * @param voxel_index
     * @param value
     */
    void setVoxel(const Eigen::Vector3i& voxel_index, Voxel& value);

    /**
     * @brief Gets the spatial position of the voxel at (i, j, k).
     * @param voxel_index
     * @return vector of spatial position of voxel at (i, j, k)
     */
    Eigen::Vector3d getVoxelPosition(const Eigen::Vector3i& voxel_index);

    /**
     * @brief Checks if a spatial position is within the voxel grid.
     * @param position
     * @return true if position is within voxel grid, false otherwise
     */
    bool withinGrid(const Eigen::Vector3d& position) const;

    /**
     * @brief Gets the voxel index of a spatial position.
     * @param position
     * @return voxel index of position
     */
    Eigen::Vector3i getVoxelIndex(const Eigen::Vector3d& position) const;

    /**
     * @brief Gets vector of material names in voxel grid.
     *
     * @return vector of material names in voxel grid
     */
    std::vector<std::string> getMaterialNames() const;

    /**
     * @brief Gets the total energy deposited in the voxel grid.
     *
     * @return total energy deposited in the voxel grid (eV)
     */
    double getTotalEnergyDeposited();

    /**
     * @brief Gets the total energy deposited in the voxel grid by material.
     *
     * @return unordered map of material name to total energy deposited in the voxel grid by material (eV)
     */
    std::unordered_map<int, VectorValue> getEnergyDepositedInMaterials();

    /**
     * @brief Adds to the number of photons that have exited the voxel grid.
     */
    void addExit() {
        numExits_++;
    }

    /**
     * @brief Gets the number of photons that have exited the voxel grid.
     * @return number of photons that have exited the voxel grid
     */
    int getNumExits() const {
        return numExits_;
    }

    /**
     * @brief Gets the spatial dimensions of the voxel grid.
     * @return vector of spatial dimensions of the voxel grid
     */
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
