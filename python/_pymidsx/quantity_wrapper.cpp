#include "quantity_wrapper.h"

namespace py = pybind11;

void wrapQuantity(py::module& m) {
    wrapQuantityContainer(m);

    py::class_<Quantity, std::unique_ptr<Quantity, py::nodelete>>(m, "Quantity")
            .def("getName", &Quantity::getName);

    py::class_<EnergyDeposition, Quantity>(m, "EnergyDeposition")
            .def(py::init<>())
            .def("measure", &EnergyDeposition::measure)
            .def("getValue", &EnergyDeposition::getValue);

    py::class_<IncidentEnergy, Quantity>(m, "IncidentEnergy")
            .def(py::init<>())
            .def("measure", &IncidentEnergy::measure)
            .def("getValue", &IncidentEnergy::getValue);

    py::class_<PrimaryIncidentEnergy, Quantity>(m, "PrimaryIncidentEnergy")
            .def(py::init<>())
            .def("measure", &PrimaryIncidentEnergy::measure)
            .def("getValue", &PrimaryIncidentEnergy::getValue);

    py::class_<SecondaryIncidentEnergy, Quantity>(m, "SecondaryIncidentEnergy")
            .def(py::init<>())
            .def("measure", &SecondaryIncidentEnergy::measure)
            .def("getValue", &SecondaryIncidentEnergy::getValue);

    py::class_<NumberOfParticles, Quantity>(m, "NumberOfParticles")
            .def(py::init<>())
            .def("measure", &NumberOfParticles::measure)
            .def("getValue", &NumberOfParticles::getValue);

    py::class_<NumberOfPrimaryParticles, Quantity>(m, "NumberOfPrimaryParticles")
            .def(py::init<>())
            .def("measure", &NumberOfPrimaryParticles::measure)
            .def("getValue", &NumberOfPrimaryParticles::getValue);

    py::class_<NumberOfSecondaryParticles, Quantity>(m, "NumberOfSecondaryParticles")
            .def(py::init<>())
            .def("measure", &NumberOfSecondaryParticles::measure)
            .def("getValue", &NumberOfSecondaryParticles::getValue);

    py::class_<EntranceCosines, Quantity>(m, "EntranceCosines")
            .def(py::init<>())
            .def("measure", &EntranceCosines::measure)
            .def("getValue", &EntranceCosines::getValue);

    py::class_<PrimaryEntranceCosines, Quantity>(m, "PrimaryEntranceCosines")
            .def(py::init<>())
            .def("measure", &PrimaryEntranceCosines::measure)
            .def("getValue", &PrimaryEntranceCosines::getValue);

    py::class_<NumberOfInteractions, Quantity>(m, "NumberOfInteractions")
            .def(py::init<>())
            .def("measure", &NumberOfInteractions::measure)
            .def("getValue", &NumberOfInteractions::getValue);
}