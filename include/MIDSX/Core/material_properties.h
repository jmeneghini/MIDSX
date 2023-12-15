#ifndef MCXRAYTRANSPORT_MATERIAL_PROPERTIES_H
#define MCXRAYTRANSPORT_MATERIAL_PROPERTIES_H

#include "data_access_object.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

/**
 * @brief Class which represents the properties of a material.
 *
 * This class contains the properties of a material obtained and calculated from the SQLite database.
 */
class MaterialProperties {
public:
    /**
     * @brief Constructor for the MaterialProperties class.
     *
     * @param name The name of the material.
     * @param dao The DataAccessObject to use to query the database.
     */
    MaterialProperties(std::string  name, DataAccessObject& dao);

    /**
     * @brief Returns the name of the material.
     *
     * @return The name of the material.
     */
    std::string getName() const { return name_; }

    /**
     * @brief Returns the ID of the material.
     *
     * @return The ID of the material.
     */
    int getMaterialId() const { return material_id_; }

    /**
     * @brief Returns the mass density of the material.
     *
     * @return The mass density of the material.
     */
    double getMassDensity() const { return mass_density_; }

    /**
     * @brief Returns the number density of the material.
     *
     * @return The number density of the material.
     */
    double getNumberDensity() const { return number_density_; }

    /**
     * @brief Returns the atomic weight of the material.
     *
     * @return The atomic weight of the material.
     */
    double getAtomicWeight() const { return atomic_weight_; }

    /**
     * @brief Returns the fractional elemental composition of the material.
     *
     * @return The elemental composition of the material as a map of element ID to fractional composition.
     */
    std::unordered_map<int, double> getElementalComposition() const { return elemental_composition_; }

    /**
     * @brief Returns the mass density of the elements in the material.
     *
     * @return The mass density of the elements in the material as a map of element ID to mass density.
     */
    std::unordered_map<int, double> getElementalMassDensity() const { return elemental_mass_density_; }

    /**
     * @brief Returns the number density of the elements in the material.
     *
     * @return The number density of the elements in the material as a map of element ID to number density.
     */
    std::unordered_map<int, double> getElementalNumberDensity() const { return elemental_number_density_; }

    /**
     * @brief Returns the atomic weight of the elements in the material.
     *
     * @return The atomic weight of the elements in the material as a map of element ID to atomic weight.
     */
    std::unordered_map<int, double> getElementalAtomicWeight() const { return elemental_atomic_weight_; }

    /**
     * @brief Returns the mass number of the elements in the material.
     *
     * @return The mass number of the elements in the material as a map of element ID to mass number.
     */
    std::unordered_map<int, double> getElementalMassNumber() const { return elemental_mass_number_; }
private:
    std::string name_;
    DataAccessObject& dao_;
    int material_id_;
    double mass_density_;
    double number_density_;
    double atomic_weight_;
    std::unordered_map<int, double> elemental_composition_;
    std::unordered_map<int, double> elemental_mass_density_;
    std::unordered_map<int, double> elemental_number_density_;
    std::unordered_map<int, double> elemental_atomic_weight_;
    std::unordered_map<int, double> elemental_mass_number_;


    void initializeProperties();

    void setMaterialId();
    void setElementalComposition();
    void initializeMaterialProperties();



    void setElementalDataForTableAndColumn(const std::string &table_name, const std::string &column_name,
                                           std::unordered_map<int, double> &map);

    void setMassDensity();
    void setNumberDensity();
    void setAtomicWeight();
};

#endif //MCXRAYTRANSPORT_MATERIAL_PROPERTIES_H
