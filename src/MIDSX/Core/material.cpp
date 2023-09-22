#include "Core/material.h"


Material::Material(std::string  name, DataAccessObject& dao) : name_(std::move(name)), dao_(dao),
                                                                     properties_(name_, dao_), data_(properties_, dao_) {
//    initializeMaterial();
}