#ifndef MCXRAYTRANSPORT_MATERIAL_PROPERTIES_H
#define MCXRAYTRANSPORT_MATERIAL_PROPERTIES_H

#include "data_access_object.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>


class MaterialProperties {
public:
    MaterialProperties(std::string  name, DataAccessObject& dao);

    std::string getName() const { return name_; }
    int getMaterialId() const { return material_id_; }
    double getMassDensity() const { return mass_density_; }
    double getNumberDensity() const { return number_density_; }
    double getAtomicWeight() const { return atomic_weight_; }
    std::unordered_map<int, double> getElementalComposition() const { return elemental_composition_; }
    std::unordered_map<int, double> getElementalMassDensity() const { return elemental_mass_density_; }
    std::unordered_map<int, double> getElementalNumberDensity() const { return elemental_number_density_; }
    std::unordered_map<int, double> getElementalAtomicWeight() const { return elemental_atomic_weight_; }
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
