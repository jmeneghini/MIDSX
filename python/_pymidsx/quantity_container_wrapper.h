#ifndef MIDSX_QUANTITY_CONTAINER_WRAPPER_H
#define MIDSX_QUANTITY_CONTAINER_WRAPPER_H

#include <pybind11/pybind11.h>
#include <MIDSX/Core.h>

namespace py = pybind11;

void wrapQuantityContainer(py::module& m);

#endif //MIDSX_QUANTITY_CONTAINER_WRAPPER_H
