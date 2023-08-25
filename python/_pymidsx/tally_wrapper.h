#ifndef MIDSX_TALLY_WRAPPER_H
#define MIDSX_TALLY_WRAPPER_H

#include <MIDSX/Core.h>
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

void wrapTally(py::module& m);

#endif //MIDSX_TALLY_WRAPPER_H
