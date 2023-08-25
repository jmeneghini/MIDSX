#include "tally_wrapper.h"

namespace py = pybind11;

void wrapTally(py::module& m) {
    py::class_<Tally, std::shared_ptr<Tally>>(m, "Tally", "Base class for all tallies.")
            .def("process_measurements", &Tally::processMeasurements, py::arg("temp_tally_data"),
                 "Process the measurements using the provided temporary tally data.")
            .def("get_quantity_container", &Tally::getQuantityContainer,
                 "Retrieve the QuantityContainer associated with the tally.");


    py::class_<DiscSurfaceTally, Tally, std::shared_ptr<DiscSurfaceTally>>(m, "DiscSurfaceTally", "Tally for a disc-shaped surface.")
            .def(py::init([](const Eigen::Vector3d& center, const Eigen::Vector3d& norm, double radius, QuantityContainer* quantities) {
                     return new DiscSurfaceTally(center, norm, radius, std::unique_ptr<QuantityContainer>(quantities));
                 }), py::arg("center"), py::arg("norm"), py::arg("radius"), py::arg("quantities"),
                 "Construct a DiscSurfaceTally with the specified parameters.");

    py::class_<RectangularSurfaceTally, Tally, std::shared_ptr<RectangularSurfaceTally>>(m, "RectangularSurfaceTally", "Tally for a rectangular surface.")
            .def(py::init([](const Eigen::Vector3d& corner, const Eigen::Vector3d& edge1, const Eigen::Vector3d& edge2, QuantityContainer* quantities) {
                     return new RectangularSurfaceTally(corner, edge1, edge2, std::unique_ptr<QuantityContainer>(quantities));
                 }), py::arg("corner"), py::arg("edge1"), py::arg("edge2"), py::arg("quantities"),
                 "Construct a RectangularSurfaceTally with the specified parameters.");

    py::class_<TallyData>(m, "TallyData", "Data structure for storing results from a tally.")
            .def(py::init<>(), "Default constructor for TallyData.")

            .def_readwrite("energy_deposition", &TallyData::energy_deposition, "Energy deposition vector.")
            .def_readwrite("incident_energy", &TallyData::incident_energy, "Incident energy vector.")
            .def_readwrite("primary_incident_energy", &TallyData::primary_incident_energy, "Primary incident energy vector.")
            .def_readwrite("secondary_incident_energy", &TallyData::secondary_incident_energy, "Secondary incident energy vector.")

            .def_readwrite("number_of_particles", &TallyData::number_of_particles, "Total number of particles.")
            .def_readwrite("number_of_primary_particles", &TallyData::number_of_primary_particles, "Number of primary particles.")
            .def_readwrite("number_of_secondary_particles", &TallyData::number_of_secondary_particles, "Number of secondary particles.")

            .def_readwrite("entrance_cosines", &TallyData::entrance_cosines, "Entrance cosines vector.")
            .def_readwrite("primary_entrance_cosines", &TallyData::primary_entrance_cosines, "Primary entrance cosines vector.")

            .def_readwrite("number_of_interactions", &TallyData::number_of_interactions, "Number of interactions.")
            .def_readwrite("area", &TallyData::area, "Area of the tally.");

}