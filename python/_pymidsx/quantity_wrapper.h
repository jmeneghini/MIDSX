#ifndef MIDSX_QUANTITY_WRAPPER_H
#define MIDSX_QUANTITY_WRAPPER_H

#include <pybind11/numpy.h>
#include <MIDSX/Core.h>
#include "quantity_container_wrapper.h"

void wrapQuantity(py::module& m);

#endif //MIDSX_QUANTITY_WRAPPER_H
