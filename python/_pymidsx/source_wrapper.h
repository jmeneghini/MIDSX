#ifndef MIDSX_SOURCE_WRAPPER_H
#define MIDSX_SOURCE_WRAPPER_H

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include <MIDSX/Core.h>

namespace py = pybind11;

void wrapSource(py::module& m);

#endif //MIDSX_SOURCE_WRAPPER_H
