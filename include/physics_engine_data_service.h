#ifndef MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
#define MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H

#include "helper.h"
#include "data_access_object.h"

class PhysicsEngineDataService {
public:
    explicit PhysicsEngineDataService(const std::string& db_name) : dao_(db_name) {}

    std::map<int, std::map<std::string, Eigen::MatrixXd>> getCrossSections(const std::vector<int>& elements, const std::pair<double, double>& energy_range = {0, 1E20}) {
        std::map<int, std::map<std::string, Eigen::MatrixXd>> data;

        for (const auto &element : elements) {
            std::string scatter_query = "SELECT Energy, CrossSection FROM IncoherentScatteringCrossSections WHERE ElementID = " + std::to_string(element)
                                        + " AND Energy BETWEEN " + std::to_string(energy_range.first) + " AND " + std::to_string(energy_range.second);
            std::vector<double> res = convertStringVector<double>(dao_.executeQuery(scatter_query));
            auto res_distributed = distributeNTimes(res, 2);

            Eigen::MatrixXd incoherent_cross_sections_matrix = convertNVectorsToEigenMatrix(res_distributed);

            std::string coherent_query = "SELECT Energy, CrossSection FROM CoherentScatteringCrossSections WHERE ElementID = " + std::to_string(element)
                                         + " AND Energy BETWEEN " + std::to_string(energy_range.first) + " AND " + std::to_string(energy_range.second);
            res = convertStringVector<double>(dao_.executeQuery(coherent_query));
            res_distributed = distributeNTimes(res, 2);

            Eigen::MatrixXd coherent_cross_sections_matrix = convertNVectorsToEigenMatrix(res_distributed);

            std::string photo_query = "SELECT Energy, CrossSection FROM TotalPhotoIonizationCrossSections WHERE ElementID = " + std::to_string(element)
                                      + " AND Energy BETWEEN " + std::to_string(energy_range.first) + " AND " + std::to_string(energy_range.second);
            res = convertStringVector<double>(dao_.executeQuery(photo_query));
            res_distributed = distributeNTimes(res, 2);

            Eigen::MatrixXd photo_cross_sections_matrix = convertNVectorsToEigenMatrix(res_distributed);

            data[element] = {
                    {"incoherent", incoherent_cross_sections_matrix},
                    {"coherent", coherent_cross_sections_matrix},
                    {"photoelectric", photo_cross_sections_matrix}
            };
        }
        return data;
    }

    std::map<std::string, int> getZs(const std::vector<std::string>& elements) {
        std::map<std::string, int> data;
        for (const auto &element : elements) {
            std::string query = "SELECT Z FROM elements WHERE element = '" + element + "'";
            std::vector<std::string> res = dao_.executeQuery(query);
            data[element] = std::stoi(res[0]);
        }
        return data;
    }

    std::map<int, Eigen::MatrixXd> getIncoherentScatteringFunctions(const std::vector<int>& elements, const std::pair<double, double>& energy_range = {0, 1E20}) {
        std::map<int, Eigen::MatrixXd> data;
        for (const auto &element : elements) {
            std::string query = "SELECT Energy, ScatteringFunction FROM IncoherentScatteringFunctions WHERE ElementID = " + std::to_string(element)
                                + " AND Energy BETWEEN " + std::to_string(energy_range.first) + " AND " + std::to_string(energy_range.second);
            std::vector<double> res = convertStringVector<double>(dao_.executeQuery(query));
            auto res_distributed = distributeNTimes(res, 2);
            Eigen::MatrixXd scattering_functions_matrix = convertNVectorsToEigenMatrix(res_distributed);
            data[element] = scattering_functions_matrix;
        }
        return data;
    }

    std::map<int, Eigen::MatrixXd> getCoherentScatteringFormFactors(const std::vector<int>& elements, const std::pair<double, double>& energy_range = {0, 1E20}) {
        std::map<int, Eigen::MatrixXd> data;
        for (const auto &element : elements) {
            std::string query = "SELECT Energy, FormFactor FROM CoherentScatteringFormFactors WHERE ElementID = " + std::to_string(element)
                                + " AND Energy BETWEEN " + std::to_string(energy_range.first) + " AND " + std::to_string(energy_range.second);
            std::vector<double> res = convertStringVector<double>(dao_.executeQuery(query));
            auto res_distributed = distributeNTimes(res, 2);
            Eigen::MatrixXd form_factors_matrix = convertNVectorsToEigenMatrix(res_distributed);
            data[element] = form_factors_matrix;
        }
        return data;
    }

private:
    DataAccessObject dao_;
};

#endif //MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
