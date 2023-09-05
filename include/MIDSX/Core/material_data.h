#ifndef MCXRAYTRANSPORT_MATERIAL_DATA_H
#define MCXRAYTRANSPORT_MATERIAL_DATA_H

#include "data_access_object.h"
#include "material_properties.h"
#include "material_helpers.h"
#include "interpolators.h"
#include <string>
#include <memory>
#include <Eigen/Dense>

class MaterialData {
public:
    MaterialData(std::shared_ptr<MaterialProperties> properties, std::shared_ptr<DataAccessObject> dao);

    Eigen::Matrix<double, Eigen::Dynamic, 2> getIncoherentScatteringCrossSectionMatrix() const { return incoherent_cs_matrix_; }
    Eigen::Matrix<double, Eigen::Dynamic, 2> getCoherentScatteringCrossSectionMatrix() const { return coherent_cs_matrix_; }
    Eigen::Matrix<double, Eigen::Dynamic, 2> getPhotoelectricCrossSectionMatrix() const { return photoelectric_cs_matrix_; }
    Eigen::Matrix<double, Eigen::Dynamic, 2> getTotalCrossSectionMatrix() const { return total_cs_matrix_; }

    Eigen::Matrix<double, Eigen::Dynamic, 2> getIncoherentScatteringFunctionMatrix() const { return incoherent_scattering_function_matrix_; }
    Eigen::Matrix<double, Eigen::Dynamic, 2> getCoherentFormFactorMatrix() const { return coherent_form_factor_matrix_; }

    double interpolateIncoherentScatteringCrossSection(double energy) const { return (*incoherent_cs_interpolator_)(energy); }
    double interpolateCoherentScatteringCrossSection(double energy) const { return (*coherent_cs_interpolator_)(energy); }
    double interpolatePhotoelectricCrossSection(double energy) const { return (*photoelectric_cs_interpolator_)(energy); }
    double interpolateTotalCrossSection(double energy) const { return (*total_cs_interpolator_)(energy); }

    double interpolateIncoherentScatteringFunction(double energy) const { return (*incoherent_scattering_function_interpolator_)(energy); }
    double interpolateCoherentFormFactor(double energy) const { return (*coherent_form_factor_interpolator_)(energy); }
    double interpolateMassEnergyAbsorptionCoefficient(double energy) const { return (*mass_energy_absorption_coefficient_interpolator_)(energy); }
private:
    std::shared_ptr<MaterialProperties> properties_;
    std::shared_ptr<DataAccessObject> dao_;

    Eigen::Matrix<double, Eigen::Dynamic, 2> incoherent_cs_matrix_;
    std::shared_ptr<Interpolator::LogLogSpline> incoherent_cs_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> coherent_cs_matrix_;
    std::shared_ptr<Interpolator::LogLogSpline> coherent_cs_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> photoelectric_cs_matrix_;
    std::shared_ptr<Interpolator::LogLogLinear> photoelectric_cs_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> total_cs_matrix_;
    std::shared_ptr<Interpolator::LogLogLinear> total_cs_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> incoherent_scattering_function_matrix_;
    std::shared_ptr<Interpolator::LogLogLinear> incoherent_scattering_function_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> coherent_form_factor_matrix_;
    std::shared_ptr<Interpolator::LogLogLinear> coherent_form_factor_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> mass_energy_absorption_coefficient_matrix_;
    std::shared_ptr<Interpolator::LogLogLinear> mass_energy_absorption_coefficient_interpolator_;

    void initializeData();

    void setInteractionCrossSectionsAndInterpolators();
    void setTotalCrossSectionsAndInterpolator();
    void setIncoherentScatteringFunctionAndInterpolator();
    void setCoherentScatteringFormFactorAndInterpolator();
    void setMassEnergyAbsorptionCoefficientsAndInterpolator();

    void setIncoherentScatteringCrossSectionAndInterpolator();
    void setCoherentScatteringCrossSectionAndInterpolator();
    void setPhotoelectricCrossSectionAndInterpolator();

    Eigen::Matrix<double, Eigen::Dynamic, 2> getTotalCrossSectionsMatrixFromInteractionData();
    Eigen::Matrix<double, Eigen::Dynamic, 2> calculateWeightedAverageOfColumns(const std::string &tableName, const std::string &dataColumnName,
                                                                               bool scale_to_macroscopic = false);

    void fillTotalCrossSectionsMatrix(Eigen::MatrixXd& total_cross_sections_matrix, const Eigen::MatrixXd& merged_energy_matrix);
    std::unordered_map<int, Eigen::Matrix<double, Eigen::Dynamic, 2>> getTableMatrixForAllElements(const std::string &tableName, const std::string &dataColumnName);
    std::unordered_map<int, std::shared_ptr<Interpolator::Interpolator>> getInterpolatorsForAllElements(const std::string &tableName,
            const std::unordered_map<int, Eigen::Matrix<double, Eigen::Dynamic, 2>>& table_matrix_map);

    static std::shared_ptr<Interpolator::Interpolator> getInterpolatorForElement(const std::string& tableName, const Eigen::Matrix<double, Eigen::Dynamic, 2>& matrix);

    Eigen::Matrix<double, Eigen::Dynamic, 2> getTableMatrix(const std::string& tableName, const std::string& dataColumnName, int element);
};

#endif //MCXRAYTRANSPORT_MATERIAL_DATA_H
