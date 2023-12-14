#ifndef MCXRAYTRANSPORT_MATERIAL_H
#define MCXRAYTRANSPORT_MATERIAL_H

#include "interpolators.h"
#include "data_access_object.h"
#include "material_properties.h"
#include "material_data.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>

/**
 * @brief Class which represents a material.
 *
 * This class contains the properties and data for a material obtained and computed from the SQLite database.
 */
class Material {
public:
    /**
     * @brief Constructor for the Material class.
     *
     * @param name The name of the material.
     * @param dao The DataAccessObject to use to query the database.
     */
    Material(std::string name, DataAccessObject& dao);

    /**
     * @brief Returns the MaterialProperties object for the material.
     *
     * @return The MaterialProperties object for the material.
     */
    const MaterialProperties& getProperties() { return properties_;}

    /**
     * @brief Returns the MaterialData object for the material.
     *
     * @return The MaterialData object for the material.
     */
    const MaterialData& getData() { return data_;}
private:
    std::string name_;
    DataAccessObject& dao_;
    MaterialProperties properties_;
    MaterialData data_;
};

#endif //MCXRAYTRANSPORT_MATERIAL_H
