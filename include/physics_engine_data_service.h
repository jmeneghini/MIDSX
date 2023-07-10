#ifndef MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
#define MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H

#include "helper.h"
#include "data_access_object.h"
#include "interpolators.h"
#include <iostream>
#include <memory>

namespace PhysicsEngineDataServiceHelpers {
    template <typename T>
    inline std::vector<T> castStringVector(const std::vector<std::string>& string_vector);

    Eigen::MatrixXd mergeMatrices(std::vector<Eigen::MatrixXd>& matrices);

    template <typename T>
    Eigen::MatrixXd convertNVectorsToEigenMatrix(const std::vector<std::vector<T>>& source);

    template <typename T>
    std::vector<std::vector<T>> distributeNTimes(const std::vector<T>& source, int n);
}

struct InteractionDataForElement {
    Eigen::Matrix<double, Eigen::Dynamic, 2> incoherent_cs_matrix;
    std::shared_ptr<Interpolator::LogLogSpline> incoherent_cs_interpolator;

    Eigen::Matrix<double, Eigen::Dynamic, 2> coherent_cs_matrix;
    std::shared_ptr<Interpolator::LogLogSpline> coherent_cs_interpolator;

    Eigen::Matrix<double, Eigen::Dynamic, 2> photoelectric_cs_matrix;
    std::shared_ptr<Interpolator::LogLogLinear> photoelectric_cs_interpolator;

    Eigen::Matrix<double, Eigen::Dynamic, 2> total_cs_matrix;
    std::shared_ptr<Interpolator::LogLogLinear> total_cs_interpolator;

    Eigen::Matrix<double, Eigen::Dynamic, 2> incoherent_scattering_function_matrix;
    std::shared_ptr<Interpolator::LogLogLinear> incoherent_scattering_function_interpolator;

    Eigen::Matrix<double, Eigen::Dynamic, 2> coherent_form_factor_matrix;
    std::shared_ptr<Interpolator::LogLogLinear> coherent_form_factor_interpolator;

    double number_density = 0.0;
};

struct InteractionData {
    std::map<int, InteractionDataForElement> interaction_data_map;
    Eigen::Matrix<double, Eigen::Dynamic, 2> max_total_cs_matrix;
    std::shared_ptr<Interpolator::LogLogLinear> max_total_cs_interpolator;
};




class PhysicsEngineDataService {
public:
    explicit PhysicsEngineDataService(const std::string& db_name, const std::vector<int>& elements,
                                      const std::pair<double, double>& energy_range = {0, 1E20});

    // Getter methods for accessing the precomputed data
    const InteractionData& getInteractionData() const;

private:
    DataAccessObject dao_;
    // Store the data in member variables
    InteractionData interaction_data_;

    void initializeData(const std::vector<int>& elements, const std::pair<double, double>& energy_range);
    Eigen::Matrix<double, Eigen::Dynamic, 2> getTableMatrix(const std::string& tableName, const std::string& dataColumnName, int element, const std::pair<double, double>& energy_range);

    double getNumberDensity(int element);


    static Eigen::Matrix<double, Eigen::Dynamic, 2> getTotalCrossSectionsMatrixFromInteractionData(const InteractionDataForElement& interaction_data);

    Eigen::Matrix<double, Eigen::Dynamic, 2> getTotalMaxCrossSectionsMatrixFromInteractionData(const std::vector<int>& elements);
};

#endif //MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
