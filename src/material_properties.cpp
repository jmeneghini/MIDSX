#include "material_properties.h"

#include <utility>
#include "data_access_object.h"
#include "constants.h"
#include "interaction_data.h"

MaterialProperties::MaterialProperties(std::string name, std::shared_ptr<DataAccessObject> dao) : name_(std::move(name)), dao_(std::move(dao)) {
    initializeProperties();
}

void MaterialProperties::initializeProperties() {
    setMaterialId();
    setElementalComposition();
    initializeElementalProperties();
    initializeMaterialProperties();
}

void MaterialProperties::setMaterialId() {
    std::string query = "SELECT MaterialID FROM Materials WHERE Name = '" + name_ + "';";
    material_id_ = InteractionDataHelpers::castStringVector<int>(dao_->executeQuery(query))[0];
}

void MaterialProperties::setElementalComposition() {
    std::string query = "SELECT MaterialCompositions.ElementID, MaterialCompositions.WeightFraction "
                        "FROM Materials "
                        "INNER JOIN MaterialCompositions ON Materials.MaterialID = MaterialCompositions.MaterialID "
                        "WHERE Materials.Name = '" + name_ + "';";
    auto output = InteractionDataHelpers::castStringVector<double>(dao_->executeQuery(query));
    auto output_distributed = InteractionDataHelpers::distributeNTimes(output, 2);
    for (int i = 0; i < output_distributed[0].size(); ++i) {
        elemental_composition_[static_cast<int>(output_distributed[0][i])] = output_distributed[1][i];
    }
}

void MaterialProperties::initializeElementalProperties() {
    setElementalDataForTableAndColumn("Elements", "Mass", elemental_atomic_weight_);
    setElementalDataForTableAndColumn("Elements", "Density", elemental_mass_density_);
    setElementalDataForTableAndColumn("Elements", "NumberDensity", elemental_number_density_);
}

void MaterialProperties::initializeMaterialProperties() {
    setMassDensity();
    if (name_ == "Air, Dry (near sea level)") {
        number_density_ = NUMBER_DENSITY_AT_STP; // only gas in database, made an exception, not proud of it
    }
    else {
        number_density_ = MaterialHelpers::calculateWeightedAverage(MaterialHelpers::mapElementsToVector(elemental_composition_), MaterialHelpers::mapElementsToVector(elemental_number_density_));
    }
    atomic_weight_ = MaterialHelpers::calculateWeightedAverage(MaterialHelpers::mapElementsToVector(elemental_composition_), MaterialHelpers::mapElementsToVector(elemental_atomic_weight_));
}


void MaterialProperties::setElementalDataForTableAndColumn(const std::string& table_name, const std::string& column_name, std::unordered_map<int, double>& map) {
    std::vector<int> element_ids = MaterialHelpers::mapKeysToVector(elemental_composition_);
    std::string element_ids_str = MaterialHelpers::mapKeysToString(elemental_composition_);
    std::string query = "SELECT AtomicNumber, " + column_name + " "
                        "FROM " + table_name + " "
                        "WHERE ID IN (" + element_ids_str + ");";
    auto output = InteractionDataHelpers::castStringVector<double>(dao_->executeQuery(query));
    auto output_distributed = InteractionDataHelpers::distributeNTimes(output, 2);
    for (int i = 0; i < output_distributed[0].size(); ++i) {
        map[static_cast<int>(output_distributed[0][i])] = output_distributed[1][i];
    }
}

void MaterialProperties::setMassDensity() {
    std::string query = "SELECT Density "
                        "FROM Materials "
                        "WHERE Name = '" + name_ + "';";
    auto output = InteractionDataHelpers::castStringVector<double>(dao_->executeQuery(query));
    mass_density_ = static_cast<double>(output[0]);
}



