#include "Core/interaction_data.h"

InteractionData::InteractionData(const std::vector<std::string>& material_names) :
        dao_(DataAccessObject(MIDSX_DB_PATH)), material_names_(material_names) {
    initializeData();
}

std::string InteractionData::getAnyMaterialNameFromId(int id) {
    std::string material_name = InteractionDataHelpers::convertMaterialIdToName(id, dao_);
    return material_name;
}

uint8_t InteractionData::getAnyMaterialIdFromName(std::string name) {
    uint8_t material_id = InteractionDataHelpers::convertMaterialNameToId(name, dao_);
    return material_id;
}

void InteractionData::initializeData() {
    setMaterialMap();
    setMaxTotalCrossSectionsAndInterpolator();
}

void InteractionData::setMaterialMap() {
    for (const auto& material_name : material_names_) {
        Material temp_material(material_name, dao_);
        material_map_.emplace(temp_material.getProperties().getMaterialId(), temp_material);
    }
}


void InteractionData::setMaxTotalCrossSectionsAndInterpolator() {
    max_total_cs_matrix_ = getTotalMaxCrossSectionsMatrixFromInteractionData();
    max_total_cs_interpolator_ = Interpolator::LogLogLinear(max_total_cs_matrix_);
}

Eigen::Matrix<double, Eigen::Dynamic, 2> InteractionData::getTotalMaxCrossSectionsMatrixFromInteractionData() {
    // create a vector of all energy matrices for each element
    std::vector<Eigen::MatrixXd> all_energies;
    for (auto& material : material_map_) {
        all_energies.emplace_back(material.second.getData().getTotalCrossSectionMatrix().col(0));
    }
    // merge the energy column vectors into a single matrix
    Eigen::MatrixXd merged_energy_matrix = InteractionDataHelpers::mergeMatrices(all_energies);

    // loop through energy range and look for the maximum cross section by looking at each element type
    Eigen::MatrixXd total_max_cross_sections_matrix = Eigen::MatrixXd::Zero(merged_energy_matrix.rows(), 2);
    fillTotalMaxCrossSectionsMatrix(total_max_cross_sections_matrix, merged_energy_matrix);
    return total_max_cross_sections_matrix;
}

void InteractionData::fillTotalMaxCrossSectionsMatrix(Eigen::MatrixXd& total_max_cross_sections_matrix, const Eigen::MatrixXd& merged_energy_matrix) {
    for (int i = 0; i < merged_energy_matrix.rows(); ++i) {
        double energy = merged_energy_matrix(i, 0);
        std::vector<double> total_cross_section_for_each_element;
        for (auto& material : material_map_) {
            total_cross_section_for_each_element.emplace_back(material.second.getData().interpolateTotalCrossSection(energy));
        }
        double max_cross_section = *std::max_element(total_cross_section_for_each_element.begin(), total_cross_section_for_each_element.end());
        total_max_cross_sections_matrix(i, 0) = energy;
        total_max_cross_sections_matrix(i, 1) = max_cross_section;
    }
}