/**
 * @file interaction_data.h
 */
#ifndef MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
#define MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H

#include "data_access_object.h"
#include "interpolators.h"
#include "material.h"
#include "config.h"
#include <iostream>
#include <memory>
#include <map>

namespace {
    // anonymous namespace. only accessible in this file
    /**
     * @brief Relative path to the EPDL database.
     */
    const std::string MIDSX_DB_DIR_VAR = MIDSX_DB_DIR;
    const std::string MIDSX_DB_NAME = "midsx.db";
    const std::string MIDSX_DB_PATH = MIDSX_DB_DIR_VAR + "/" + MIDSX_DB_NAME;
}

/**
 * @brief Class which provides access to various simulation significant data.
 *
 * This class provides an interface to the database along with Material objects which contain computed data, along with data that is computed from all materials.
 */
class InteractionData {
public:
    /**
     * @brief Constructor for the InteractionData class.
     *
     * @param material_names A vector of strings containing the names of the materials to be used in the simulation.
     */
    explicit InteractionData(const std::vector<std::string>& material_names);

    /**
     * @brief Returns the Material object with the given ID.
     *
     * @param id The ID of the Material object to return.
     * @return The Material object with the given ID.
     */
    Material& getMaterialFromId(int id) { return material_map_.at(id); }

    /**
     * @brief Returns the 2 column matrix containing the maximum total cross sections for all materials.
     *
     * The first column contains the energy values, and the second column contains the maximum total cross sections.
     * Energy values are in eV and cross sections are in cm^-1.
     *
     * @return The 2 column matrix containing the maximum total cross sections for all materials.
     */
    Eigen::Matrix<double, Eigen::Dynamic, 2> getMaxTotalCrossSectionsMatrix() const { return max_total_cs_matrix_; }

    /**
     * @brief Returns the maximum total cross section for all materials at the given energy.
     *
     * @param energy The energy (eV) at which to return the maximum total cross section.
     * @return The maximum total cross section (cm^-1) for all materials at the given energy .
     */
    double interpolateMaxTotalCrossSection(double energy) const { return (max_total_cs_interpolator_)(energy); }

    /**
     * @brief Returns the name of the material with the given ID.
     *
     * @param id The ID of the material.
     * @return The name of the material with the given ID.
     */
    std::string getAnyMaterialNameFromId(int id);

    /**
     * @brief Returns the ID of the material with the given name.
     *
     * @param name The name of the material.
     * @return The ID of the material with the given name.
     */
    uint8_t getAnyMaterialIdFromName(std::string name);
private:
    DataAccessObject dao_;
    std::vector<std::string> material_names_;
    std::map<int, Material> material_map_;
    Eigen::Matrix<double, Eigen::Dynamic, 2> max_total_cs_matrix_;
    Interpolator::LogLogLinear max_total_cs_interpolator_;

    /**
     * @brief Initializes the data for the InteractionData object.
     */
    void initializeData();

    /**
     * @brief Sets the material map for the InteractionData object.
     *
     * The material map is a map of material IDs to Material objects.
     */
    void setMaterialMap();

    /**
     * @brief Sets the maximum total cross sections and interpolator for the InteractionData object.
     */
    void setMaxTotalCrossSectionsAndInterpolator();

    /**
     * @brief Returns the 2 column matrix containing the maximum total cross sections for all materials.
     *
     * The first column contains the energy values, and the second column contains the maximum total cross sections.
     * Energy values are in eV and cross sections are in cm^-1.
     *
     * @return The 2 column matrix containing the maximum total cross sections for all materials.
     */
    Eigen::Matrix<double, Eigen::Dynamic, 2> getTotalMaxCrossSectionsMatrixFromInteractionData();

    /**
     * @brief Fills the total max cross sections matrix with the maximum total cross sections for all materials.
     *
     * @param total_max_cross_sections_matrix The matrix to fill with the maximum total cross sections for all materials.
     * @param merged_energy_matrix The matrix containing the energy values for all materials.
     */
    void fillTotalMaxCrossSectionsMatrix(Eigen::MatrixXd& total_max_cross_sections_matrix, const Eigen::MatrixXd& merged_energy_matrix);
};

#endif //MC_XRAY_TRANSPORT_PHYSICS_ENGINE_DATA_SERVICE_H
