#include <pybind11/pybind11.h>
#include "interaction_data_wrapper.h"
#include "voxel_grid_wrapper.h"
#include "quantity_wrapper.h"
#include "tally_wrapper.h"
#include "physics_engine_wrapper.h"
#include "source_wrapper.h"
#include "run_simulation_wrapper.h"

namespace py = pybind11;

PYBIND11_MODULE(pymidsx, m) {
    m.doc() = "MIDSX: Monte Carlo Interaction and Dosage Simulation of X-rays";

    // main module bindings
    wrapInteractionData(m);
    wrapVoxelGrid(m);
    wrapTally(m);
    wrapPhysicsEngine(m);
    wrapSource(m);
    wrapRunSimulation(m);

    // submodules
    py::module quantity = m.def_submodule("quantity", "Quantity module");
    wrapQuantity(quantity);
}