#include "Core/material.h"


Material::Material(std::string  name, std::shared_ptr<DataAccessObject> dao) : name_(std::move(name)), dao_(std::move(dao)) {
    initializeMaterial();
}

void Material::initializeMaterial() {
    properties_ = std::make_shared<MaterialProperties>(name_, dao_);
    data_ = std::make_shared<MaterialData>(properties_, dao_);
}