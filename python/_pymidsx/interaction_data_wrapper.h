#ifndef INTERACTION_DATA_WRAPPER_H
#define INTERACTION_DATA_WRAPPER_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include <MIDSX/Core.h>

namespace py = pybind11;

inline std::shared_ptr<DataAccessObject> setupDataService(const std::string& db_path);

inline std::vector<std::shared_ptr<Material>> getMaterialsFromListOfNames(
        const py::list& materials, const std::shared_ptr<DataAccessObject>& data_service);

void wrapInteractionData(py::module& m);

#endif