#ifndef MCXRAYTRANSPORT_MATERIAL_DATA_H
#define MCXRAYTRANSPORT_MATERIAL_DATA_H

#include "data_access_object.h"
#include "material_properties.h"
#include "material_helpers.h"
#include "interpolators.h"
#include "probability_dist.h"
#include "constants.h"
#include <string>
#include <memory>
#include <Eigen/Dense>

/**
 * @brief Class which contains the computed simulation data for a material.
 *
 * This class contains the computed data for a material obtained from the SQLite database.
 */
class MaterialData {
public:
    /**
     * @brief Constructor for the MaterialData class.
     *
     * @param properties The MaterialProperties object for the material.
     * @param dao The DataAccessObject to use to query the database.
     */
    MaterialData(MaterialProperties& properties, DataAccessObject& dao);

    /**
     * @brief Returns the 2 column matrix containing the incoherent scattering cross sections for the material.
     *
     * The first column contains the energy values, and the second column contains the incoherent scattering cross sections.
     * Energy values are in eV and cross sections are in cm^-1.
     *
     * @return The 2 column matrix containing the incoherent scattering cross sections for the material.
     */
    Eigen::Matrix<double, Eigen::Dynamic, 2> getIncoherentScatteringCrossSectionMatrix() const { return incoherent_cs_matrix_; }

    /**
     * @brief Returns the 2 column matrix containing the coherent scattering cross sections for the material.
     *
     * The first column contains the energy values, and the second column contains the coherent scattering cross sections.
     * Energy values are in eV and cross sections are in cm^-1.
     *
     * @return The 2 column matrix containing the coherent scattering cross sections for the material.
     */
    Eigen::Matrix<double, Eigen::Dynamic, 2> getCoherentScatteringCrossSectionMatrix() const { return coherent_cs_matrix_; }

    /**
     * @brief Returns the 2 column matrix containing the photoelectric cross sections for the material.
     *
     * The first column contains the energy values, and the second column contains the photoelectric cross sections.
     * Energy values are in eV and cross sections are in cm^-1.
     *
     * @return The 2 column matrix containing the photoelectric cross sections for the material.
     */
    Eigen::Matrix<double, Eigen::Dynamic, 2> getPhotoelectricCrossSectionMatrix() const { return photoelectric_cs_matrix_; }

    /**
     * @brief Returns the 2 column matrix containing the total cross sections for the material.
     *
     * The first column contains the energy values, and the second column contains the total cross sections.
     * Energy values are in eV and cross sections are in cm^-1.
     *
     * @return The 2 column matrix containing the total cross sections for the material.
     */
    Eigen::Matrix<double, Eigen::Dynamic, 2> getTotalCrossSectionMatrix() const { return total_cs_matrix_; }

    /**
     * @brief Returns the 2 column matrix containing the incoherent scattering functions for the material.
     *
     * The first column contains the momentum transfer values, and the second column contains the incoherent scattering function.
     * Momentum transfer values are in inverse angstroms and the scattering function is unitless.
     *
     * @return The 2 column matrix containing the incoherent scattering functions for the material.
     */
    Eigen::Matrix<double, Eigen::Dynamic, 2> getIncoherentScatteringFunctionMatrix() const { return incoherent_scattering_function_matrix_; }

    /**
     * @brief Returns the 2 column matrix containing the coherent scattering form factors for the material.
     *
     * The first column contains the momentum transfer values, and the second column contains the coherent scattering form factor.
     * Momentum transfer values are in inverse angstroms and the form factor is unitless.
     *
     * @return The 2 column matrix containing the coherent scattering form factors for the material.
     */
    Eigen::Matrix<double, Eigen::Dynamic, 2> getCoherentFormFactorMatrix() const { return coherent_form_factor_matrix_; }

    /**
     * @brief Returns the interpolated incoherent scattering cross section for the material at the given energy.
     *
     * @param energy The energy (eV) at which to return the incoherent scattering cross section.
     * @return The interpolated incoherent scattering cross section (cm^-1) for the material at the given energy.
     */
    double interpolateIncoherentScatteringCrossSection(double energy) const { return (incoherent_cs_interpolator_)(energy); }

    /**
     * @brief Returns the interpolated coherent scattering cross section for the material at the given energy.
     *
     * @param energy The energy (eV) at which to return the coherent scattering cross section.
     * @return The interpolated coherent scattering cross section (cm^-1) for the material at the given energy.
     */
    double interpolateCoherentScatteringCrossSection(double energy) const { return (coherent_cs_interpolator_)(energy); }

    /**
     * @brief Returns the interpolated photoelectric cross section for the material at the given energy.
     *
     * @param energy The energy (eV) at which to return the photoelectric cross section.
     * @return The interpolated photoelectric cross section (cm^-1) for the material at the given energy.
     */
    double interpolatePhotoelectricCrossSection(double energy) const { return (photoelectric_cs_interpolator_)(energy); }

    /**
     * @brief Returns the interpolated total cross section for the material at the given energy.
     *
     * @param energy The energy (eV) at which to return the total cross section.
     * @return The interpolated total cross section (cm^-1) for the material at the given energy.
     */
    double interpolateTotalCrossSection(double energy) const { return (total_cs_interpolator_)(energy); }

    /**
     * @brief Returns the interpolated incoherent scattering function for the material at the given momentum transfer.
     *
     * @param x The momentum transfer (inverse angstroms) at which to return the incoherent scattering function.
     * @return The interpolated incoherent scattering function (unitless) for the material at the given momentum transfer.
     */
    double interpolateIncoherentScatteringFunction(double x) const { return (incoherent_scattering_function_interpolator_)(x); }

    /**
     * @brief Returns the interpolated coherent scattering form factor for the material at the given momentum transfer.
     *
     * @param x The momentum transfer (inverse angstroms) at which to return the coherent scattering form factor.
     * @return The interpolated coherent scattering form factor (unitless) for the material at the given momentum transfer.
     */
    double interpolateCoherentFormFactor(double x) const { return (coherent_form_factor_interpolator_)(x); }

    /**
     * @brief Returns the interpolated mass energy absorption coefficient for the material at the given energy.
     *
     * @param energy The energy (eV) at which to return the mass energy absorption coefficient.
     * @return The interpolated mass energy absorption coefficient (cm^2/g) for the material at the given energy.
     */
    double interpolateMassEnergyAbsorptionCoefficient(double energy) const { return (mass_energy_absorption_coefficient_interpolator_)(energy); }

    /**
     * @brief Samples the incoherent scattering DCS distribution for the material at the given energy.
     *
     * @param energy The energy (eV) at which to sample the incoherent scattering DCS distribution.
     * @return The sampled incoherent scattering DCS for the material at the given energy.
     */
    double sampleCoherentScatteringDCS(double energy) const { return coherent_scattering_dcs_dist_.sample(energy); }

private:
    MaterialProperties& properties_;
    DataAccessObject& dao_;

    Eigen::Matrix<double, Eigen::Dynamic, 2> incoherent_cs_matrix_;
    Interpolator::LogLogSpline incoherent_cs_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> coherent_cs_matrix_;
    Interpolator::LogLogSpline coherent_cs_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> photoelectric_cs_matrix_;
    Interpolator::LogLogLinear photoelectric_cs_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> total_cs_matrix_;
    Interpolator::LogLogLinear total_cs_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> incoherent_scattering_function_matrix_;
    Interpolator::LogLogLinear incoherent_scattering_function_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> coherent_form_factor_matrix_;
    Interpolator::LogLogLinear coherent_form_factor_interpolator_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> mass_energy_absorption_coefficient_matrix_;
    Interpolator::LogLogLinear mass_energy_absorption_coefficient_interpolator_;
    ProbabilityDist::ContinuousInversion coherent_scattering_dcs_dist_;

    void initializeData();

    void setInteractionCrossSectionsAndInterpolators();
    void setTotalCrossSectionsAndInterpolator();
    void setIncoherentScatteringFunctionAndInterpolator();
    void setCoherentScatteringFormFactorAndInterpolator();
    void setMassEnergyAbsorptionCoefficientsAndInterpolator();

    void setIncoherentScatteringCrossSectionAndInterpolator();
    void setCoherentScatteringCrossSectionAndInterpolator();
    void setPhotoelectricCrossSectionAndInterpolator();

    void setCoherentScatteringDCSDistribution();

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
