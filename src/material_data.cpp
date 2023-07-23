#include "material_data.h"

#include <utility>

MaterialData::MaterialData(std::shared_ptr<MaterialProperties> properties, std::shared_ptr<DataAccessObject> dao) :
        properties_(std::move(properties)), dao_(std::move(dao)) {
    initializeData();
}

void MaterialData::initializeData() {
    setInteractionCrossSectionsAndInterpolators();
    setTotalCrossSectionsAndInterpolator();
    setIncoherentScatteringFunctionAndInterpolator();
    setCoherentScatteringFormFactorAndInterpolator();
}

void MaterialData::setInteractionCrossSectionsAndInterpolators() {
    setIncoherentScatteringCrossSectionAndInterpolator();
    setCoherentScatteringCrossSectionAndInterpolator();
    setPhotoelectricCrossSectionAndInterpolator();
}

void MaterialData::setTotalCrossSectionsAndInterpolator() {
    total_cs_matrix_ = getTotalCrossSectionsMatrixFromInteractionData();
    total_cs_interpolator_ = std::make_shared<Interpolator::LogLogLinear>(total_cs_matrix_);
}


void MaterialData::setIncoherentScatteringFunctionAndInterpolator() {
    incoherent_scattering_function_matrix_ = calculateWeightedAverageOfColumns("IncoherentScatteringFunctions", "ScatteringFunction");
    incoherent_scattering_function_interpolator_ = std::make_shared<Interpolator::LogLogLinear>(incoherent_scattering_function_matrix_);
}



void MaterialData::setCoherentScatteringFormFactorAndInterpolator() {
    coherent_form_factor_matrix_ = calculateWeightedAverageOfColumns("CoherentScatteringFormFactors", "FormFactor");
    coherent_form_factor_interpolator_ = std::make_shared<Interpolator::LogLogLinear>(coherent_form_factor_matrix_);
}

void MaterialData::setIncoherentScatteringCrossSectionAndInterpolator() {
    incoherent_cs_matrix_ = calculateWeightedAverageOfColumns("IncoherentScatteringCrossSections", "CrossSection");
    incoherent_cs_matrix_.col(1) *= 1E-24 * properties_->getNumberDensity(); // Convert from barns to cm^2 then to the macroscopic cross section
    incoherent_cs_interpolator_ = std::make_shared<Interpolator::LogLogSpline>(incoherent_cs_matrix_);
}

void MaterialData::setCoherentScatteringCrossSectionAndInterpolator() {
    coherent_cs_matrix_ = calculateWeightedAverageOfColumns("CoherentScatteringCrossSections", "CrossSection");
    coherent_cs_matrix_.col(1) *= 1E-24 * properties_->getNumberDensity();
    coherent_cs_interpolator_ = std::make_shared<Interpolator::LogLogSpline>(coherent_cs_matrix_);
}

void MaterialData::setPhotoelectricCrossSectionAndInterpolator() {
    photoelectric_cs_matrix_ = calculateWeightedAverageOfColumns("TotalPhotoIonizationCrossSections", "CrossSection");
    photoelectric_cs_matrix_.col(1) *= 1E-24 * properties_->getNumberDensity();
    photoelectric_cs_interpolator_ = std::make_shared<Interpolator::LogLogLinear>(photoelectric_cs_matrix_);
}

Eigen::Matrix<double, Eigen::Dynamic, 2> MaterialData::getTotalCrossSectionsMatrixFromInteractionData() {
    // create a vector of all energy matrices
    std::vector<Eigen::MatrixXd> all_energies = {incoherent_cs_matrix_.col(0),
                                                 coherent_cs_matrix_.col(0),
                                                 photoelectric_cs_matrix_.col(0)};
    // merge the energy column vectors into a single matrix
    Eigen::MatrixXd merged_energy_matrix = InteractionDataHelpers::mergeMatrices(all_energies);
    Eigen::MatrixXd total_cross_sections_matrix = Eigen::MatrixXd::Zero(merged_energy_matrix.rows(), 2);
    fillTotalCrossSectionsMatrix(total_cross_sections_matrix, merged_energy_matrix);
    return total_cross_sections_matrix;
}

Eigen::Matrix<double, Eigen::Dynamic, 2> MaterialData::calculateWeightedAverageOfColumns(const std::string &tableName, const std::string &dataColumnName) {
    auto elemental_composition = MaterialHelpers::mapElementsToVector(properties_->getElementalComposition());
    auto matrices = getTableMatrixForAllElements(tableName, dataColumnName);
    Eigen::Matrix<double, Eigen::Dynamic, 2> weighted_average_matrix(matrices[0].rows(), 2);
    weighted_average_matrix.col(0) = matrices[0].col(0);
    weighted_average_matrix.col(1) = Eigen::VectorXd::Zero(matrices[0].rows());
    for (int i = 0; i < elemental_composition.size(); ++i) {
        weighted_average_matrix.col(1) += elemental_composition[i] * matrices[i].col(1);
    }
    return weighted_average_matrix;
} // TODO: Assumed energies were the same for all elements, but this is not the case. Need to fix this.

std::vector<Eigen::Matrix<double, Eigen::Dynamic, 2>> MaterialData::getTableMatrixForAllElements(const std::string &tableName, const std::string &dataColumnName) {
    std::vector<int> element_ids = MaterialHelpers::mapKeysToVector(properties_->getElementalComposition());
    std::vector<Eigen::Matrix<double, Eigen::Dynamic, 2>> matrices;
    for (int element : element_ids) {
        matrices.push_back(getTableMatrix(tableName, dataColumnName, element));
    }
    return matrices;
}

void MaterialData::fillTotalCrossSectionsMatrix(Eigen::MatrixXd& total_cross_sections_matrix, const Eigen::MatrixXd& merged_energy_matrix) {
    for (int i = 0; i < merged_energy_matrix.rows(); ++i) {
        double energy = merged_energy_matrix(i, 0);
        double incoherent_cross_section = (*incoherent_cs_interpolator_)(energy);
        double coherent_cross_section = (*coherent_cs_interpolator_)(energy);
        double photo_cross_section = (*photoelectric_cs_interpolator_)(energy);
        total_cross_sections_matrix(i, 0) = energy;
        total_cross_sections_matrix(i, 1) = incoherent_cross_section + coherent_cross_section + photo_cross_section;
    }
}

Eigen::Matrix<double, Eigen::Dynamic, 2> MaterialData::getTableMatrix(const std::string &tableName,
                                                                      const std::string &dataColumnName, int element) {
    std::string query = "SELECT Energy, " + dataColumnName + " FROM " + tableName + " WHERE ElementID = " + std::to_string(element);
    std::vector<double> res = InteractionDataHelpers::castStringVector<double>(dao_->executeQuery(query));
    auto res_distributed = InteractionDataHelpers::distributeNTimes(res, 2);

    return InteractionDataHelpers::convertNVectorsToEigenMatrix(res_distributed);
}








