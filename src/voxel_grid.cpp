#include <iostream>
#include "voxel_grid.h"



template <typename T, std::size_t N>
Eigen::Vector<T, N> VoxelGridHelpers::pyTupleToEigenVector(const py::tuple &tuple) {
    Eigen::Vector<T, N> vector;
    for (std::size_t i = 0; i < N; i++) {
        vector[i] = tuple[i].cast<T>();
    }
    return vector;
}

double VoxelGridHelpers::covertUnitNameToCm(const std::string& unitName) {
    std::unordered_map<std::string, double> unitMap = {
            {"nm", 1e-7},
            {"um", 1e-4},
            {"mm", 0.1},
            {"cm", 1.0},
            {"m", 100.0},
            {"km", 1e5},
    }; // converts to cm
    return unitMap[unitName];
}

int VoxelGridHelpers::dimensionStringToIndex(const std::string& dimension) {
    std::unordered_map<std::string, int> dimensionMap = {
            {"x", 0},
            {"y", 1},
            {"z", 2},
    };
    return dimensionMap[dimension];
}



VoxelGrid::VoxelGrid(const std::string& nii_filename,
                     const Eigen::Vector3d& origin):
                     filename_(nii_filename), origin_(origin) {
    initializeVoxels();
};
// initialize voxels to default values

// get voxel value at (i, j, k)
Voxel& VoxelGrid::getVoxel(const Eigen::Vector3i& voxel_index) {
    return voxels_.at(voxelNumber(voxel_index));
}

void VoxelGrid::setVoxel(const Eigen::Vector3i& voxel_index, Voxel &value) {
    voxels_.at(voxelNumber(voxel_index)) = value;
}

Eigen::Vector3d VoxelGrid::getVoxelPosition(const Eigen::Vector3i& voxel_index) {
    return voxel_index.cast<double>().cwiseProduct(spacing_); // element-wise multiplication
}

Eigen::Vector3i VoxelGrid::getVoxelIndex(const Eigen::Vector3d& position) {
    handleOutOfBounds(position);
    Eigen::Vector3i voxelIndex = (position.cwiseQuotient(spacing_)).cast<int>();
    return voxelIndex;
}

double VoxelGrid::getTotalEnergyDeposited() {
    double totalDose = 0.0;
    for (auto& voxel : voxels_) {
        totalDose += voxel.dose;
    }
    return totalDose;
}

Eigen::Matrix<double, Eigen::Dynamic, 2> VoxelGrid::getEnergyDepositedAlongAxis(const std::string &axis) {
    Eigen::Matrix<double, Eigen::Dynamic, 2> energyDepositedAlongAxis;
    int axis_index = VoxelGridHelpers::dimensionStringToIndex(axis);
    int numVoxelsAlongAxis = dim_vox_[axis_index];
    energyDepositedAlongAxis.resize(numVoxelsAlongAxis, 2);
    // get the energy deposited along the axis
    int two_other_axes[2] = { (axis_index + 1) % 3, (axis_index + 2) % 3 };
    for (int i = 0; i < numVoxelsAlongAxis; i++) {
        Eigen::Vector3i index = Eigen::Vector3i::Zero();
        index[axis_index] = i;
        for (int j = 0; j < dim_vox_[two_other_axes[0]]; j++) {
            for (int k = 0; k < dim_vox_[two_other_axes[1]]; k++) {
                index[two_other_axes[0]] = j;
                index[two_other_axes[1]] = k;
                Voxel& voxel = getVoxel(index);
                energyDepositedAlongAxis(i, 1) += voxel.dose;
            }
        }
        energyDepositedAlongAxis(i, 0) = i;
    }
    return energyDepositedAlongAxis;
} // TODO: clean up this function. ALSO... IT SEEMS IM GETTING MEMORY ERRORS WHEN I DO THE Y-AXIS BUT NOT WHEN IN ISOLATION?????
// I'VE BEEN TRYING TO PICK THIS APART FOR 3 HRS STRAIGHT. MAYBE IT'S TIME TO SWITCH TO BUSINESS

std::unordered_map<int, double> VoxelGrid::getEnergyDepositedInMaterials() {
    std::unordered_map<int, double> energyDepositedInMaterials;
    for (auto& voxel : voxels_) {
        if (energyDepositedInMaterials.find(voxel.materialID) == energyDepositedInMaterials.end()) {
            energyDepositedInMaterials[voxel.materialID] = voxel.dose;
        } else {
            energyDepositedInMaterials[voxel.materialID] += voxel.dose;
        }
    }
    return energyDepositedInMaterials;
}

void VoxelGrid::initializeVoxels() {
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    py::module nibabel = py::module::import("nibabel"); // import nibabel
    py::object img = nibabel.attr("load")(filename_); // load the nifti file into img
    py::object data = img.attr("get_fdata")(); // get the data from img
    py::object header = img.attr("header");
    auto numpy_array = getNumPyArrayFromImg(img);
    setVoxelProperties(numpy_array, header);
    setVoxelMaterialIDs(numpy_array);
}

int VoxelGrid::voxelNumber(const Eigen::Vector3i& voxel_index) const {
    return voxel_index[0] + voxel_index[1]*dim_vox_[0] + voxel_index[2]*dim_vox_[0]*dim_vox_[1];
}

bool VoxelGrid::withinGrid(const Eigen::Vector3d& position) const {
    Eigen::Vector3d minPosition = origin_;
    Eigen::Vector3d maxPosition = origin_ + dim_space_;
    return (position.array() >= minPosition.array()).all()
           && (position.array() <= maxPosition.array()).all();
}


void VoxelGrid::handleOutOfBounds(const Eigen::Vector3d& position) const {
    if (!withinGrid(position)) {
        throw std::out_of_range("VoxelGrid::handleOutOfBounds: voxel index out of range");
    }
}

void VoxelGrid::setVoxelProperties(const py::array_t<uint8_t> &array, const py::object &header) {
    setDimVox(array);
    setUnits(header);
    setNumOfVoxels();
    setSpacing(header);
    setDimSpace();
}

py::array_t<uint8_t> VoxelGrid::getNumPyArrayFromImg(const py::object &img) {
    py::object data = img.attr("get_fdata")(); // get the data from img
    return py::cast<py::array_t<uint8_t>>(data);
}

void VoxelGrid::setDimVox(const py::array_t<uint8_t>& array) {
    py::tuple shape = array.attr("shape");
    dim_vox_ = VoxelGridHelpers::pyTupleToEigenVector<int, 3>(shape);
}

void VoxelGrid::setUnits(const py::object &header) {
    py::object units = header.attr("get_xyzt_units")();
    py::tuple unitTuple = py::cast<py::tuple>(units);
    std::string unitName = py::cast<std::string>(unitTuple[0]);
    units_ = VoxelGridHelpers::covertUnitNameToCm(unitName);
}

void VoxelGrid::setSpacing(const py::object& header) {
    py::tuple zooms = header.attr("get_zooms")();
    spacing_ = VoxelGridHelpers::pyTupleToEigenVector<double, 3>(zooms) * units_;
}

void VoxelGrid::setNumOfVoxels() {
    numOfVoxels_ = dim_vox_[0]*dim_vox_[1]*dim_vox_[2];
}

void VoxelGrid::setDimSpace() {
    dim_space_ = dim_vox_.cast<double>().cwiseProduct(spacing_);
}

void VoxelGrid::setVoxelMaterialIDs(const py::array_t<uint8_t> &array) {
    uint8_t* data_ptr = (uint8_t *) array.request().ptr;
    for (int i = 0; i < numOfVoxels_; i++) {
        Voxel voxel;
        voxel.materialID = data_ptr[i];
        voxels_.push_back(voxel);
    }
}
