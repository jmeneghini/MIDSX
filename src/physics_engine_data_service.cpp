#include "physics_engine_data_service.h"

template <typename T>
inline std::vector<T> PhysicsEngineDataServiceHelpers::castStringVector(const std::vector<std::string>& string_vector) {
    // Create a vector of the new type
    std::vector<T> converted_vector(string_vector.size());

    // Convert each string to the new type
    std::transform(string_vector.begin(), string_vector.end(), converted_vector.begin(),
                   [](const std::string& str) {
                       std::istringstream iss(str);
                       T value;
                       iss >> value;
                       return value;
                   }
    );

    return converted_vector;
}

Eigen::MatrixXd PhysicsEngineDataServiceHelpers::mergeMatrices(std::vector<Eigen::MatrixXd>& matrices) {
    std::vector<double> merged;

    for (const auto& matrix : matrices) {
        std::vector<double> v(matrix.data(), matrix.data() + matrix.size());
        merged.insert(merged.end(), v.begin(), v.end());
    }

    std::sort(merged.begin(), merged.end());

    auto last = std::unique(merged.begin(), merged.end());
    merged.erase(last, merged.end());

    Eigen::MatrixXd result = Eigen::Map<Eigen::MatrixXd>(merged.data(), merged.size(), 1);

    return result;
}

template <typename T>
Eigen::MatrixXd PhysicsEngineDataServiceHelpers::convertNVectorsToEigenMatrix(const std::vector<std::vector<T>>& source) {
    Eigen::MatrixXd result(source[0].size(), source.size());

    for (int i = 0; i < source.size(); ++i) {
        result.col(i) = Eigen::Map<const Eigen::VectorXd>(source[i].data(), source[i].size());
    }

    return result;
}

template <typename T>
std::vector<std::vector<T>> PhysicsEngineDataServiceHelpers::distributeNTimes(const std::vector<T>& source, int n) {
    std::vector<std::vector<T>> result(n);  // Initialize vector of vectors

    for (int i = 0; i < source.size(); ++i) {
        result[i % n].push_back(source[i]);
    }

    return result;
}



PhysicsEngineDataService::PhysicsEngineDataService(const std::string& db_name, const std::vector<int>& elements,
                                                   const std::pair<double, double>& energy_range) : dao_(db_name), elements_(elements), energy_range_(energy_range)
{
    // Initialize the data as soon as the service is created
    initializeData();
}

const InteractionData& PhysicsEngineDataService::getInteractionData() const {
    return interaction_data_;
}

void PhysicsEngineDataService::initializeData() {
    for (const auto &element : elements_) {
        InteractionDataForElement interaction_data;
        setInteractionCrossSectionsAndInterpolators(interaction_data, element);
        setTotalCrossSectionsAndInterpolator(interaction_data);
        setIncoherentScatteringFunctionAndInterpolator(interaction_data, element);
        setCoherentScatteringFormFactorAndInterpolator(interaction_data, element);
        setNumberDensity(interaction_data, element);
        moveInteractionDataToMap(interaction_data, element);
    }
    setMaxTotalCrossSectionsAndInterpolator();
}

void PhysicsEngineDataService::setInteractionCrossSectionsAndInterpolators(InteractionDataForElement& interaction_data, int element) {
    setIncoherentScatteringCrossSectionAndInterpolator(interaction_data, element);
    setCoherentScatteringCrossSectionAndInterpolator(interaction_data, element);
    setPhotoelectricCrossSectionAndInterpolator(interaction_data, element);
}

void PhysicsEngineDataService::setTotalCrossSectionsAndInterpolator(InteractionDataForElement& interaction_data) {
    interaction_data.total_cs_matrix = getTotalCrossSectionsMatrixFromInteractionData(interaction_data);
    interaction_data.total_cs_interpolator = std::make_shared<Interpolator::LogLogLinear>(interaction_data.total_cs_matrix);
}

void PhysicsEngineDataService::setIncoherentScatteringCrossSectionAndInterpolator(InteractionDataForElement& interaction_data, int element) {
    interaction_data.incoherent_cs_matrix = getTableMatrix("IncoherentScatteringCrossSections", "CrossSection", element);
    interaction_data.incoherent_cs_matrix.col(1) *= 1E-24; // Convert from barns to cm^2
    interaction_data.incoherent_cs_interpolator = std::make_shared<Interpolator::LogLogSpline>(interaction_data.incoherent_cs_matrix);
}

void PhysicsEngineDataService::setCoherentScatteringCrossSectionAndInterpolator(InteractionDataForElement& interaction_data, int element) {
    interaction_data.coherent_cs_matrix = getTableMatrix("CoherentScatteringCrossSections", "CrossSection", element);
    interaction_data.coherent_cs_matrix.col(1) *= 1E-24;
    interaction_data.coherent_cs_interpolator = std::make_shared<Interpolator::LogLogSpline>(interaction_data.coherent_cs_matrix);
}

void PhysicsEngineDataService::setPhotoelectricCrossSectionAndInterpolator(InteractionDataForElement& interaction_data, int element) {
    interaction_data.photoelectric_cs_matrix = getTableMatrix("TotalPhotoIonizationCrossSections", "CrossSection", element);
    interaction_data.photoelectric_cs_matrix.col(1) *= 1E-24;
    interaction_data.photoelectric_cs_interpolator = std::make_shared<Interpolator::LogLogLinear>(interaction_data.photoelectric_cs_matrix);
}

void PhysicsEngineDataService::setCoherentScatteringFormFactorAndInterpolator(InteractionDataForElement& interaction_data, int element) {
    interaction_data.coherent_form_factor_matrix = getTableMatrix("CoherentScatteringFormFactors", "FormFactor", element);
    interaction_data.coherent_form_factor_matrix.col(1) = interaction_data.coherent_form_factor_matrix.col(1).array().pow(2); // form factor is only ever used squared, so do it here
    interaction_data.coherent_form_factor_interpolator = std::make_shared<Interpolator::LogLogLinear>(interaction_data.coherent_form_factor_matrix);
}

void PhysicsEngineDataService::setIncoherentScatteringFunctionAndInterpolator(InteractionDataForElement& interaction_data, int element) {
    interaction_data.incoherent_scattering_function_matrix = getTableMatrix("IncoherentScatteringFunctions", "ScatteringFunction", element);
    interaction_data.incoherent_scattering_function_interpolator = std::make_shared<Interpolator::LogLogLinear>(interaction_data.incoherent_scattering_function_matrix);

}

void PhysicsEngineDataService::setNumberDensity(InteractionDataForElement& interaction_data, int element) {
    interaction_data.number_density = getNumberDensity(element);
}

void PhysicsEngineDataService::moveInteractionDataToMap(InteractionDataForElement& interaction_data, int element) {
    interaction_data_.interaction_data_map.emplace(element, std::move(interaction_data));
}

void PhysicsEngineDataService::setMaxTotalCrossSectionsAndInterpolator() {
    interaction_data_.max_total_cs_matrix = getTotalMaxCrossSectionsMatrixFromInteractionData();
    interaction_data_.max_total_cs_interpolator = std::make_shared<Interpolator::LogLogLinear>(interaction_data_.max_total_cs_matrix);
}




Eigen::Matrix<double, Eigen::Dynamic, 2> PhysicsEngineDataService::getTableMatrix(const std::string& tableName, const std::string& dataColumnName, int element) {
    std::string query = "SELECT Energy, " + dataColumnName + " FROM " + tableName + " WHERE ElementID = " + std::to_string(element)
                        + " AND Energy BETWEEN " + std::to_string(energy_range_.first) + " AND " + std::to_string(energy_range_.second);
    std::vector<double> res = PhysicsEngineDataServiceHelpers::castStringVector<double>(dao_.executeQuery(query));
    auto res_distributed = PhysicsEngineDataServiceHelpers::distributeNTimes(res, 2);

    return PhysicsEngineDataServiceHelpers::convertNVectorsToEigenMatrix(res_distributed);
}

double PhysicsEngineDataService::getNumberDensity(int element) {
    std::string query = "SELECT NumberDensity FROM Elements WHERE AtomicNumber = " + std::to_string(element);
    std::vector<double> res = PhysicsEngineDataServiceHelpers::castStringVector<double>(dao_.executeQuery(query));
    return res[0];
}

Eigen::Matrix<double, Eigen::Dynamic, 2> PhysicsEngineDataService::getTotalCrossSectionsMatrixFromInteractionData(const InteractionDataForElement& interaction_data) {
    // create a vector of all energy matrices
    std::vector<Eigen::MatrixXd> all_energies = {interaction_data.incoherent_cs_matrix.col(0),
                                                 interaction_data.coherent_cs_matrix.col(0),
                                                 interaction_data.photoelectric_cs_matrix.col(0)};
    // merge the energy column vectors into a single matrix
    Eigen::MatrixXd merged_energy_matrix = PhysicsEngineDataServiceHelpers::mergeMatrices(all_energies);
    Eigen::MatrixXd total_cross_sections_matrix = Eigen::MatrixXd::Zero(merged_energy_matrix.rows(), 2);
    fillTotalCrossSectionsMatrix(interaction_data, total_cross_sections_matrix, merged_energy_matrix);
    return total_cross_sections_matrix;
}

Eigen::Matrix<double, Eigen::Dynamic, 2> PhysicsEngineDataService::getTotalMaxCrossSectionsMatrixFromInteractionData() {
    // create a vector of all energy matrices for each element
    std::vector<Eigen::MatrixXd> all_energies;
    for (const auto& element : elements_) {
        all_energies.emplace_back(interaction_data_.interaction_data_map[element].total_cs_matrix.col(0));
    }
    // merge the energy column vectors into a single matrix
    Eigen::MatrixXd merged_energy_matrix = PhysicsEngineDataServiceHelpers::mergeMatrices(all_energies);

    // loop through energy range and look for the maximum cross section by looking at each element type
    Eigen::MatrixXd total_max_cross_sections_matrix = Eigen::MatrixXd::Zero(merged_energy_matrix.rows(), 2);
    fillTotalMaxCrossSectionsMatrix(total_max_cross_sections_matrix, merged_energy_matrix);
    return total_max_cross_sections_matrix;
}

void PhysicsEngineDataService::fillTotalCrossSectionsMatrix(const InteractionDataForElement& interaction_data, Eigen::MatrixXd& total_cross_sections_matrix, const Eigen::MatrixXd& merged_energy_matrix) {
    for (int i = 0; i < merged_energy_matrix.rows(); ++i) {
        double energy = merged_energy_matrix(i, 0);
        double incoherent_cross_section = (*interaction_data.incoherent_cs_interpolator)(energy);
        double coherent_cross_section = (*interaction_data.coherent_cs_interpolator)(energy);
        double photo_cross_section = (*interaction_data.photoelectric_cs_interpolator)(energy);
        total_cross_sections_matrix(i, 0) = energy;
        total_cross_sections_matrix(i, 1) = incoherent_cross_section + coherent_cross_section + photo_cross_section;
    }
}

void PhysicsEngineDataService::fillTotalMaxCrossSectionsMatrix(Eigen::MatrixXd& total_max_cross_sections_matrix, const Eigen::MatrixXd& merged_energy_matrix) {
    for (int i = 0; i < merged_energy_matrix.rows(); ++i) {
        double energy = merged_energy_matrix(i, 0);
        std::vector<double> total_cross_section_for_each_element;
        for (const auto& element : elements_) {
            total_cross_section_for_each_element.emplace_back((*interaction_data_.interaction_data_map[element].total_cs_interpolator)(energy));
        }
        double max_cross_section = *std::max_element(total_cross_section_for_each_element.begin(), total_cross_section_for_each_element.end());
        total_max_cross_sections_matrix(i, 0) = energy;
        total_max_cross_sections_matrix(i, 1) = max_cross_section;
    }
}



