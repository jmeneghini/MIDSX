#include "quantity_container_wrapper.h"

namespace py = pybind11;


void wrapQuantityContainer(py::module& m) {
    py::class_<QuantityContainer>(m, "QuantityContainer", "Class to manage and operate on different Quantity objects.")
            .def(py::init<>(), "Construct an empty QuantityContainer.")
            .def(py::init([](const py::list& quantity_list) {
                     QuantityContainer quantity_container;
                     for (auto& quantity : quantity_list) {
                         std::unique_ptr<Quantity> q_ptr(quantity.cast<Quantity*>());
                         quantity_container.addQuantity(std::move(q_ptr));
                     }
                     return quantity_container;
                 }), py::arg("quantity_list"),
                 "Construct a QuantityContainer from a list of Quantity objects. \n\n"
                 "Parameters:\n"
                 "  quantity_list (list): List of Quantity objects.")

            .def("add_quantity", [](QuantityContainer& q, Quantity* quantity) {
                     std::unique_ptr<Quantity> q_ptr(quantity);
                     q.addQuantity(std::move(q_ptr));
                 }, py::arg("quantity"),
                 "Add a quantity to the container. \n\n"
                 "Parameters:\n"
                 "  quantity (Quantity): The quantity to add.")

            .def("process_measurements", &QuantityContainer::processMeasurements,
                 "Process all measurements for the quantities in the container.")

            .def("get_tally_data", &QuantityContainer::getTallyData,
                 "Retrieve the tally data associated with the container. \n\n"
                 "Returns:\n"
                 "  TallyData: The tally data.")

            .def("get_primary_fluence", &QuantityContainer::getPrimaryFluence,
                 "Retrieve the primary fluence value. \n\n"
                 "Returns:\n"
                 "  double: The primary fluence value.")

            .def("get_primary_air_kerma", &QuantityContainer::getPrimaryAirKerma, py::arg("interaction_data"), py::arg("MaterialID"), py::arg("energy_bin"),
                 "Retrieve the primary air kerma for a specific material and energy bin. \n\n"
                 "Parameters:\n"
                 "  interaction_data (InteractionData): Interaction data for the material.\n"
                 "  MaterialID (int): ID of the material.\n"
                 "  energy_bin (Eigen::VectorXd): Energy bin data.\n\n"
                 "Returns:\n"
                 "  double: The primary air kerma value.")

            .def("set_area", &QuantityContainer::setArea, py::arg("area"),
                 "Set the area for the according Tally. \n\n"
                 "Parameters:\n"
                 "  area (double): The area value to set.")

            .def("__iter__", [](QuantityContainer &q) {
                std::vector<std::pair<std::string, Quantity*>> items;
                for (auto &pair : q) {
                    items.push_back({pair.first, pair.second.get()});
                }
                return py::make_iterator(items.begin(), items.end());
            }, py::keep_alive<0, 1>(), "Allows iteration over the key-value pairs in the container.");
}
