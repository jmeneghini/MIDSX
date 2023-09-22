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

class Material {
public:
    Material(std::string name, DataAccessObject& dao);

    const MaterialProperties& getProperties() { return properties_;}
    const MaterialData& getData() { return data_;}
private:
    std::string name_;
    DataAccessObject& dao_;
    MaterialProperties properties_;
    MaterialData data_;
};

#endif //MCXRAYTRANSPORT_MATERIAL_H
