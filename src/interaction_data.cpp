#include "interaction_data.h"

InteractionData::InteractionData(const std::vector<std::shared_ptr<Material>>& materials, std::shared_ptr<DataAccessObject> dao) :
        dao_(std::move(dao)), materials_(materials)
{
    // Initialize the data as soon as the service is created
    initializeData();
}


void InteractionData::initializeData() {
    for (const auto& material : materials_) {
        material_map_[material->getProperties()->getMaterialId()] = material;
    }
    setMaxTotalCrossSectionsAndInterpolator();
}


void InteractionData::setMaxTotalCrossSectionsAndInterpolator() {
    max_total_cs_matrix_ = getTotalMaxCrossSectionsMatrixFromInteractionData();
    max_total_cs_interpolator_ = std::make_shared<Interpolator::LogLogLinear>(max_total_cs_matrix_);
}

Eigen::Matrix<double, Eigen::Dynamic, 2> InteractionData::getTotalMaxCrossSectionsMatrixFromInteractionData() {
    // create a vector of all energy matrices for each element
    std::vector<Eigen::MatrixXd> all_energies;
    for (const auto& material : materials_) {
        all_energies.emplace_back(material->getData()->getTotalCrossSectionMatrix().col(0));
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
        for (const auto& material : materials_) {
            total_cross_section_for_each_element.emplace_back(material->getData()->interpolateTotalCrossSection(energy));
        }
        double max_cross_section = *std::max_element(total_cross_section_for_each_element.begin(), total_cross_section_for_each_element.end());
        total_max_cross_sections_matrix(i, 0) = energy;
        total_max_cross_sections_matrix(i, 1) = max_cross_section;
    }
}



