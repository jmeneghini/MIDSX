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
    auto matrices_map = getTableMatrixForAllElements(tableName, dataColumnName);
    std::vector<Eigen::MatrixXd> all_energies;
    for (const auto& element : matrices_map) {
        all_energies.emplace_back(element.second.col(0));
    }
    Eigen::MatrixXd merged_energy_matrix = InteractionDataHelpers::mergeMatrices(all_energies);
    Eigen::MatrixXd weighted_average_matrix = Eigen::MatrixXd::Zero(merged_energy_matrix.rows(), 2);
    std::unordered_map<int, std::shared_ptr<Interpolator::Interpolator>> interpolators_map = getInterpolatorsForAllElements(tableName, matrices_map);
    std::unordered_map<int, double> elemental_composition = properties_->getElementalComposition();
    for (int i = 0; i < merged_energy_matrix.rows(); ++i) {
            double energy = merged_energy_matrix(i, 0);
            double weighted_average = 0;
            for (const auto& element_interpolator : interpolators_map) {
                weighted_average += elemental_composition[element_interpolator.first] * (*element_interpolator.second)(energy);
            }
            weighted_average_matrix(i, 0) = energy;
            weighted_average_matrix(i, 1) = weighted_average;
        }
    return weighted_average_matrix;
}

std::unordered_map<int, Eigen::Matrix<double, Eigen::Dynamic, 2>> MaterialData::getTableMatrixForAllElements(const std::string &tableName, const std::string &dataColumnName) {
    std::vector<int> element_ids = MaterialHelpers::mapKeysToVector(properties_->getElementalComposition());
    std::unordered_map<int, Eigen::Matrix<double, Eigen::Dynamic, 2>> matrices_map;
    for (int element : element_ids) {
        matrices_map[element] = getTableMatrix(tableName, dataColumnName, element);
    }
    return matrices_map;
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

std::unordered_map<int, std::shared_ptr<Interpolator::Interpolator>> MaterialData::getInterpolatorsForAllElements(const std::string &tableName,
        const std::unordered_map<int, Eigen::Matrix<double, Eigen::Dynamic, 2>>& table_matrix_map) {
    std::unordered_map<int, std::shared_ptr<Interpolator::Interpolator>> interpolators_map;
    for (const auto& element_matrix : table_matrix_map) {
        interpolators_map[element_matrix.first] = getInterpolatorForElement(tableName, element_matrix.second);
    }
    return interpolators_map;
}

Eigen::Matrix<double, Eigen::Dynamic, 2> MaterialData::getTableMatrix(const std::string &tableName,
                                                                      const std::string &dataColumnName, int element) {
    std::string query = "SELECT Energy, " + dataColumnName + " FROM " + tableName + " WHERE ElementID = " + std::to_string(element);
    std::vector<double> res = InteractionDataHelpers::castStringVector<double>(dao_->executeQuery(query));
    auto res_distributed = InteractionDataHelpers::distributeNTimes(res, 2);

    return InteractionDataHelpers::convertNVectorsToEigenMatrix(res_distributed);
}

std::shared_ptr<Interpolator::Interpolator> MaterialData::getInterpolatorForElement(const std::string& tableName, const Eigen::Matrix<double, Eigen::Dynamic, 2>& matrix) {
    if (tableName == "IncoherentScatteringFunctions") {
        return std::make_shared<Interpolator::LogLogLinear>(matrix);
    } else if (tableName == "CoherentScatteringFormFactors") {
        return std::make_shared<Interpolator::LogLogLinear>(matrix);
    } else if (tableName == "IncoherentScatteringCrossSections") {
        return std::make_shared<Interpolator::LogLogSpline>(matrix);
    } else if (tableName == "CoherentScatteringCrossSections") {
        return std::make_shared<Interpolator::LogLogSpline>(matrix);
    } else if (tableName == "TotalPhotoIonizationCrossSections") {
        return std::make_shared<Interpolator::LogLogLinear>(matrix);
    } else {
        throw std::runtime_error("Invalid table");
    }
}








