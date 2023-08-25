#include "voxel_grid_wrapper.h"

namespace py = pybind11;

/**
 * @brief Wraps the VoxelGrid class for Python bindings using pybind11.
 *
 * This function provides Python bindings for the VoxelGrid class, allowing it to be used in Python scripts.
 * The bindings include constructors and various member functions of the VoxelGrid class.
 *
 * @param m The pybind11 module object to which the VoxelGrid class will be added.
 */
void wrapVoxelGrid(py::module& m) {
    // Bind the VoxelGrid class to the provided module.
    py::class_<VoxelGrid>(m, "VoxelGrid")
            // Constructor binding with optional origin parameter.
            .def(py::init([](const std::string& filename, const Eigen::Vector3d& origin) {
                     return VoxelGrid(filename, origin, true);
                 }), py::arg("filename"), py::arg("origin") = Eigen::Vector3d::Zero(),
                 "Constructs a VoxelGrid object using a filename and an optional origin.")

                    // Member function bindings.
            .def("getVoxel", &VoxelGrid::getVoxel, "Returns the voxel at a specified index.")
            .def("setVoxel", &VoxelGrid::setVoxel, "Sets the voxel at a specified index.")
            .def("getVoxelPosition", &VoxelGrid::getVoxelPosition, "Returns the spatial position of a voxel at a given index.")
            .def("getVoxelIndex", &VoxelGrid::getVoxelIndex, "Returns the voxel index for a given spatial position.")
            .def("getTotalEnergyDeposited", &VoxelGrid::getTotalEnergyDeposited, "Returns the total energy deposited in the voxel grid.")
            .def("getEnergyDepositedInMaterials", &VoxelGrid::getEnergyDepositedInMaterials, "Returns the energy deposited in different materials within the voxel grid.")
            .def("addExit", &VoxelGrid::addExit, "Increments the number of photons that have exited the voxel grid.")
            .def("getNumExits", &VoxelGrid::getNumExits, "Returns the number of photons that have exited the voxel grid.")
            .def("getOrigin", &VoxelGrid::getOrigin, "Returns the origin of the voxel grid.")
            .def("getDimSpace", &VoxelGrid::getDimSpace, "Returns the dimensions of the space occupied by the voxel grid.");
    // Add other methods if needed
}
