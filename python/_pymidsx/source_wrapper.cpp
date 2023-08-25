#include "source_wrapper.h"

namespace py = pybind11;

void wrapSource(py::module& m) {
    // EnergySpectrum
    py::class_<EnergySpectrum>(m, "EnergySpectrum")
            .def("sampleEnergy", &EnergySpectrum::sampleEnergy);

    // MonoenergeticSpectrum
    py::class_<MonoenergeticSpectrum, EnergySpectrum>(m, "MonoenergeticSpectrum")
            .def(py::init<double>(), py::arg("energy"))
            .def("sampleEnergy", &MonoenergeticSpectrum::sampleEnergy);

    // PolyenergeticSpectrum
    py::class_<PolyenergeticSpectrum, EnergySpectrum>(m, "PolyenergeticSpectrum")
            .def(py::init<const Eigen::Matrix<double, Eigen::Dynamic, 2>&>(), py::arg("probabilities_matrix"))
            .def("sampleEnergy", &PolyenergeticSpectrum::sampleEnergy)
            .def("getExpectationValue", &PolyenergeticSpectrum::getExpectationValue);

    // Directionality
    py::class_<Directionality>(m, "Directionality")
            .def("sampleDirection", &Directionality::sampleDirection);

    // IsotropicDirectionality
    py::class_<IsotropicDirectionality, Directionality>(m, "IsotropicDirectionality")
            .def(py::init<>())
            .def("sampleDirection", &IsotropicDirectionality::sampleDirection);

    // BeamDirectionality
    py::class_<BeamDirectionality, Directionality>(m, "BeamDirectionality")
            .def(py::init<Eigen::Vector3d>(), py::arg("pass_through_point"))
            .def("sampleDirection", &BeamDirectionality::sampleDirection);

    // RectangularIsotropicDirectionality
    py::class_<RectangularIsotropicDirectionality, Directionality>(m, "RectangularIsotropicDirectionality")
            .def(py::init<Eigen::Vector3d, Eigen::Vector3d, Eigen::Vector3d>(), py::arg("corner"), py::arg("edge1"), py::arg("edge2"))
            .def("sampleDirection", &RectangularIsotropicDirectionality::sampleDirection);

    // SourceGeometry
    py::class_<SourceGeometry>(m, "SourceGeometry")
            .def("samplePosition", &SourceGeometry::samplePosition);

    // PointGeometry
    py::class_<PointGeometry, SourceGeometry>(m, "PointGeometry")
            .def(py::init<const Eigen::Vector3d&>(), py::arg("position"))
            .def("samplePosition", &PointGeometry::samplePosition);

    // PhotonSource
    py::class_<PhotonSource>(m, "PhotonSource")
            .def(py::init<>())
            .def(py::init([](EnergySpectrum* energy_spectrum, Directionality* directionality, SourceGeometry* geometry) {
                        auto energy_spectrum_ptr = std::unique_ptr<EnergySpectrum>(energy_spectrum);
                        auto directionality_ptr = std::unique_ptr<Directionality>(directionality);
                        auto geometry_ptr = std::unique_ptr<SourceGeometry>(geometry);
                return new PhotonSource(std::move(energy_spectrum_ptr), std::move(directionality_ptr), std::move(geometry_ptr));
            }), py::arg("energy_spectrum"), py::arg("directionality"), py::arg("geometry"))
            .def("generatePhoton", &PhotonSource::generatePhoton);
}