#include "Core/material_properties.h"

#include <utility>

#include "Core/data_access_object.h"
#include "Core/constants.h"
#include "Core/interaction_data.h"


MaterialProperties::MaterialProperties(std::string name, DataAccessObject& dao) : name_(std::move(name)), dao_(dao) {
    initializeProperties();
}

void MaterialProperties::initializeProperties() {
    setMaterialId();
    setElementalComposition();
    initializeMaterialProperties();
}

void MaterialProperties::setMaterialId() {
    material_id_ = InteractionDataHelpers::convertMaterialNameToId(name_, dao_);
}

void MaterialProperties::setElementalComposition() {
    std::string query = "SELECT MaterialCompositions.ElementID, MaterialCompositions.WeightFraction "
                        "FROM Materials "
                        "INNER JOIN MaterialCompositions ON Materials.MaterialID = MaterialCompositions.MaterialID "
                        "WHERE Materials.Name = '" + name_ + "';";
    auto output = InteractionDataHelpers::castStringVector<double>(dao_.executeQuery(query));
    auto output_distributed = InteractionDataHelpers::distributeNTimes(output, 2);
    for (int i = 0; i < output_distributed[0].size(); ++i) {
        elemental_composition_[static_cast<int>(output_distributed[0][i])] = output_distributed[1][i];
    }
}

void MaterialProperties::initializeMaterialProperties() {
    setElementalDataForTableAndColumn("Elements", "Mass", elemental_atomic_weight_);
    setElementalDataForTableAndColumn("Elements", "Density", elemental_mass_density_);
    setElementalDataForTableAndColumn("Elements", "MassNumber", elemental_mass_number_);
    setMassDensity();
    setAtomicWeight();
    setNumberDensity();
}

void MaterialProperties::setElementalDataForTableAndColumn(const std::string& table_name, const std::string& column_name, std::unordered_map<int, double>& map) {
    std::vector<int> element_ids = MaterialHelpers::mapKeysToVector(elemental_composition_);
    std::string element_ids_str = MaterialHelpers::mapKeysToString(elemental_composition_);
    std::string query = "SELECT AtomicNumber, " + column_name + " "
                        "FROM " + table_name + " "
                        "WHERE ID IN (" + element_ids_str + ");";
    auto output = InteractionDataHelpers::castStringVector<double>(dao_.executeQuery(query));
    auto output_distributed = InteractionDataHelpers::distributeNTimes(output, 2);
    for (int i = 0; i < output_distributed[0].size(); ++i) {
        map[static_cast<int>(output_distributed[0][i])] = output_distributed[1][i];
    }
}

void MaterialProperties::setMassDensity() {
    std::string query = "SELECT Density "
                        "FROM Materials "
                        "WHERE Name = '" + name_ + "';";
    auto output = InteractionDataHelpers::castStringVector<double>(dao_.executeQuery(query));
    mass_density_ = static_cast<double>(output[0]);
}

void MaterialProperties::setNumberDensity() {
    double number_density = 0;
    for (const auto& atomic_weight : elemental_atomic_weight_) {
         elemental_number_density_[atomic_weight.first] = AVOGADRO_CONSTANT * mass_density_ * elemental_composition_[atomic_weight.first] / atomic_weight.second;
         number_density += elemental_number_density_[atomic_weight.first];
    }
    number_density_ = number_density;
}

void MaterialProperties::setAtomicWeight() {
    atomic_weight_ = MaterialHelpers::calculateWeightedAverage(MaterialHelpers::mapElementsToVector(elemental_composition_),
                                                               MaterialHelpers::mapElementsToVector(elemental_atomic_weight_));
}