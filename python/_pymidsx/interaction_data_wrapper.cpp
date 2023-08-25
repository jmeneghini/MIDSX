#include "interaction_data_wrapper.h"

inline std::shared_ptr<DataAccessObject> setupDataService(const std::string& db_path) {
    return std::make_shared<DataAccessObject>(db_path);
}

inline std::vector<std::shared_ptr<Material>> getMaterialsFromListOfNames(const py::list& materials, const std::shared_ptr<DataAccessObject>& data_service) {
    std::vector<std::shared_ptr<Material>> materials_vector;
    for (auto& material : materials) {
        materials_vector.emplace_back(std::make_shared<Material>(material.cast<std::string>(), data_service));
    }
    return materials_vector;
}

void wrapInteractionData(py::module& m) {
    py::class_<InteractionData>(m, "InteractionData")
            .def(py::init<>([](const py::list& materials, const std::string& db_path) {
                auto data_service = setupDataService(db_path);
                auto materials_vector = getMaterialsFromListOfNames(materials, data_service);
                return InteractionData(materials_vector, data_service);
            }))
            .def("get_material", &InteractionData::getMaterial)
            .def("get_max_total_cross_sections_matrix", &InteractionData::getMaxTotalCrossSectionsMatrix)
            .def("interpolate_max_total_cross_section", &InteractionData::interpolateMaxTotalCrossSection);
}

