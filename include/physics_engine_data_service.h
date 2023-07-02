#ifndef MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
#define MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H

#include "helper.h"
#include "data_access_object.h"
#include "interpolators.h"
#include <memory>

struct InteractionDataForElement {
    Eigen::MatrixXd incoherent_cross_sections_matrix;
    std::shared_ptr<LogLogSplineInterpolator> incoherent_scattering_interpolator;

    Eigen::MatrixXd coherent_cross_sections_matrix;
    std::shared_ptr<LogLogSplineInterpolator> coherent_scattering_interpolator;

    Eigen::MatrixXd photo_cross_sections_matrix;
    std::shared_ptr<LinearInterpolator> photoelectric_interpolator;

    Eigen::MatrixXd total_cross_sections_matrix;
    std::shared_ptr<LinearInterpolator> total_interpolator;

    Eigen::MatrixXd tot_cross_sections_max_matrix;
    std::shared_ptr<LinearInterpolator> total_max_interpolator;

    Eigen::MatrixXd incoherent_scattering_functions_matrix;
    std::shared_ptr<LinearInterpolator> incoherent_scattering_functions_interpolator;

    Eigen::MatrixXd coherent_scattering_form_factors_matrix;
    std::shared_ptr<LinearInterpolator> coherent_scattering_form_factors_interpolator;
};

struct InteractionData {
    std::map<int, InteractionDataForElement> interaction_data_map;
    Eigen::MatrixXd total_max_cross_sections_matrix;
    std::shared_ptr<LinearInterpolator> total_max_interpolator;
};




class PhysicsEngineDataService {
public:
    explicit PhysicsEngineDataService(const std::string& db_name, const std::vector<int>& elements,
                                      const std::pair<double, double>& energy_range = {0, 1E20}) : dao_(db_name)
    {
        // Initialize the data as soon as the service is created
        initializeData(elements, energy_range);
    }

    // Getter methods for accessing the precomputed data
    const InteractionData& getInteractionData() const {
        return interaction_data_;
    }

private:
    DataAccessObject dao_;

    // Store the data in member variables
    InteractionData interaction_data_;

    void initializeData(const std::vector<int>& elements, const std::pair<double, double>& energy_range) {
        for (const auto &element : elements) {
            InteractionDataForElement interaction_data;

            interaction_data.incoherent_cross_sections_matrix = getTableMatrix("IncoherentScatteringCrossSections", "CrossSection", element, energy_range);
            interaction_data.incoherent_scattering_interpolator = std::make_shared<LogLogSplineInterpolator>(interaction_data.incoherent_cross_sections_matrix);

            interaction_data.coherent_cross_sections_matrix = getTableMatrix("CoherentScatteringCrossSections", "CrossSection", element, energy_range);
            interaction_data.coherent_scattering_interpolator = std::make_shared<LogLogSplineInterpolator>(interaction_data.coherent_cross_sections_matrix);

            interaction_data.photo_cross_sections_matrix = getTableMatrix("TotalPhotoIonizationCrossSections", "CrossSection", element, energy_range);
            interaction_data.photoelectric_interpolator = std::make_shared<LinearInterpolator>(interaction_data.photo_cross_sections_matrix);

            interaction_data.incoherent_scattering_functions_matrix = getTableMatrix("IncoherentScatteringFunctions", "ScatteringFunction", element, energy_range);
            interaction_data.incoherent_scattering_functions_interpolator = std::make_shared<LinearInterpolator>(interaction_data.incoherent_scattering_functions_matrix);

            interaction_data.coherent_scattering_form_factors_matrix = getTableMatrix("CoherentScatteringFormFactors", "FormFactor", element, energy_range);
            interaction_data.coherent_scattering_form_factors_matrix.row(1) = interaction_data.coherent_scattering_form_factors_matrix.row(1).array().pow(2); // form factor is only ever used squared, so do it here
            interaction_data.coherent_scattering_form_factors_interpolator = std::make_shared<LinearInterpolator>(interaction_data.coherent_scattering_form_factors_matrix);

            interaction_data.total_cross_sections_matrix = getTotalCrossSectionsMatrixFromInteractionData(interaction_data);
            interaction_data.total_interpolator = std::make_shared<LinearInterpolator>(interaction_data.total_cross_sections_matrix);

            interaction_data_.interaction_data_map.emplace(element, std::move(interaction_data));
        }
        interaction_data_.total_max_cross_sections_matrix = getTotalMaxCrossSectionsMatrixFromInteractionData(elements);
        interaction_data_.total_max_interpolator = std::make_shared<LinearInterpolator>(interaction_data_.total_max_cross_sections_matrix);
    }

    Eigen::MatrixXd getTableMatrix(const std::string& tableName, const std::string& dataColumnName, int element, const std::pair<double, double>& energy_range) {
        std::string query = "SELECT Energy, " + dataColumnName + " FROM " + tableName + " WHERE ElementID = " + std::to_string(element)
                            + " AND Energy BETWEEN " + std::to_string(energy_range.first) + " AND " + std::to_string(energy_range.second);
        std::vector<double> res = convertStringVector<double>(dao_.executeQuery(query));
        auto res_distributed = distributeNTimes(res, 2);

        return convertNVectorsToEigenMatrix(res_distributed);
    }


    Eigen::MatrixXd getTotalCrossSectionsMatrixFromInteractionData(const InteractionDataForElement& interaction_data) {
        // create a vector of all energy matrices
        std::vector<Eigen::MatrixXd> all_energies = {interaction_data.incoherent_cross_sections_matrix.col(0),
                                                       interaction_data.coherent_cross_sections_matrix.col(0),
                                                       interaction_data.photo_cross_sections_matrix.col(0)};
        // merge the energy column vectors into a single matrix
        Eigen::MatrixXd merged_energy_matrix = mergeMatrices(all_energies);

        // create a total cross section matrix
        Eigen::MatrixXd total_cross_sections_matrix = Eigen::MatrixXd::Zero(merged_energy_matrix.rows(), 2);

        // fill the total cross section matrix with the sum of the cross sections, for each energy, interpolating when necessary
        for (int i = 0; i < merged_energy_matrix.rows(); ++i) {
            double energy = merged_energy_matrix(i, 0);
            double incoherent_cross_section = (*interaction_data.incoherent_scattering_interpolator)(energy);
            double coherent_cross_section = (*interaction_data.incoherent_scattering_interpolator)(energy);
            double photo_cross_section = (*interaction_data.photoelectric_interpolator)(energy);
            total_cross_sections_matrix(i, 0) = energy;
            total_cross_sections_matrix(i, 1) = incoherent_cross_section + coherent_cross_section + photo_cross_section;
        }

        return total_cross_sections_matrix;
    }

    Eigen::MatrixXd getTotalMaxCrossSectionsMatrixFromInteractionData(const std::vector<int>& elements) {
        // create a vector of all energy matrices for each element
        std::vector<Eigen::MatrixXd> all_energies;
        for (const auto& element : elements) {
            all_energies.emplace_back(interaction_data_.interaction_data_map[element].total_cross_sections_matrix.col(0));
        }
        // merge the energy column vectors into a single matrix
        Eigen::MatrixXd merged_energy_matrix = mergeMatrices(all_energies);

        // loop through energy range and look for the maximum cross section by looking at each element type
        Eigen::MatrixXd total_max_cross_sections_matrix = Eigen::MatrixXd::Zero(merged_energy_matrix.rows(), 2);

        for (int i = 0; i < merged_energy_matrix.rows(); ++i) {
            double energy = merged_energy_matrix(i, 0);
            std::vector<double> total_cross_section_for_each_element;
            for (const auto& element : elements) {
                total_cross_section_for_each_element.emplace_back((*interaction_data_.interaction_data_map[element].total_interpolator)(energy));
            }
            double max_cross_section = *std::max_element(total_cross_section_for_each_element.begin(), total_cross_section_for_each_element.end());
            total_max_cross_sections_matrix(i, 0) = energy;
            total_max_cross_sections_matrix(i, 1) = max_cross_section;
        }
        return total_max_cross_sections_matrix;

    }




};

#endif //MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
