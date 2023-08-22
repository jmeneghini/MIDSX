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
    Material() = default;
    Material(std::string name, std::shared_ptr<DataAccessObject> dao);

    std::shared_ptr<MaterialProperties> getProperties() const { return properties_;}
    std::shared_ptr<MaterialData> getData() const { return data_;}
private:
    std::string name_;
    std::shared_ptr<DataAccessObject> dao_;
    std::shared_ptr<MaterialProperties> properties_;
    std::shared_ptr<MaterialData> data_;

    void initializeMaterial();
};

#endif //MCXRAYTRANSPORT_MATERIAL_H
