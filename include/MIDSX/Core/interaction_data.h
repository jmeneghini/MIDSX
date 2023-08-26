#ifndef MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
#define MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H

#include "data_access_object.h"
#include "interpolators.h"
#include "material.h"
#include <iostream>
#include <memory>
#include <map>


class InteractionData {
public:
    explicit InteractionData(std::vector<std::unique_ptr<Material>> materials, std::shared_ptr<DataAccessObject> dao);

    const Material& getMaterial(int id) { return *material_map_.at(id); }
    Eigen::Matrix<double, Eigen::Dynamic, 2> getMaxTotalCrossSectionsMatrix() const { return max_total_cs_matrix_; }
    double interpolateMaxTotalCrossSection(double energy) const { return (*max_total_cs_interpolator_)(energy); }
private:
    std::shared_ptr<DataAccessObject> dao_;
    std::vector<std::unique_ptr<Material>> materials_;
    std::map<int, std::unique_ptr<Material>> material_map_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> max_total_cs_matrix_;
    std::unique_ptr<Interpolator::LogLogLinear> max_total_cs_interpolator_;

    void initializeData();

    void setMaxTotalCrossSectionsAndInterpolator();

    Eigen::Matrix<double, Eigen::Dynamic, 2> getTotalMaxCrossSectionsMatrixFromInteractionData();
    void fillTotalMaxCrossSectionsMatrix(Eigen::MatrixXd& total_max_cross_sections_matrix, const Eigen::MatrixXd& merged_energy_matrix);
};

#endif //MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
