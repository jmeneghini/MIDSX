#include "Core/surface_quantity_container.h"

void SurfaceQuantityContainer::addVectorQuantity(const VectorSurfaceQuantity& quantity) {
    vector_quantities_.emplace(quantity.getType(), quantity);
}

void SurfaceQuantityContainer::addCountQuantity(const CountSurfaceQuantity& quantity) {
    count_quantities_.emplace(quantity.getType(), quantity);
}

void SurfaceQuantityContainer::measureAll(TempSurfaceTallyData& temp_tally_data) {
    for (auto& vector_quantity : vector_quantities_) {
        vector_quantity.second.measure(temp_tally_data);
    }
    for (auto& count_quantity : count_quantities_) {
        count_quantity.second.measure(temp_tally_data);
    }
}

SurfaceQuantityContainer SurfaceQuantityContainer::operator+(const SurfaceQuantityContainer& other) const {
    // check if either container is empty, if so return the other container
    if (vector_quantities_.empty()) {
        return other;
    }
    if (other.vector_quantities_.empty()) {
        return *this;
    }

    // check that the two containers have the same quantities
    for (auto& vector_quantity : vector_quantities_) {
        if (other.vector_quantities_.find(vector_quantity.first) == other.vector_quantities_.end()) {
            throw std::runtime_error("Cannot add SurfaceQuantityContainers with different vector quantities.");
        }
    }
    for (auto& count_quantity : count_quantities_) {
        if (other.count_quantities_.find(count_quantity.first) == other.count_quantities_.end()) {
            throw std::runtime_error("Cannot add SurfaceQuantityContainers with different count quantities.");
        }
    }
    // check that the two containers have the same area (e.g they are both the same surface tally)
    if (area_ != other.area_) {
        throw std::runtime_error("Cannot add SurfaceQuantityContainers with different areas.");
    }
    // add the quantities
    auto new_container = SurfaceQuantityContainer();
    new_container.setArea(area_);
    for (auto& vector_quantity : vector_quantities_) {
        new_container.addVectorQuantity(vector_quantity.second + other.vector_quantities_.at(vector_quantity.first));
    }
    for (auto& count_quantity : count_quantities_) {
        new_container.addCountQuantity(count_quantity.second + other.count_quantities_.at(count_quantity.first));
    }
    return new_container;
}

std::unordered_map<VectorSurfaceQuantityType, VectorSurfaceQuantity>& SurfaceQuantityContainer::getVectorQuantities() {
    return vector_quantities_;
}

std::unordered_map<CountSurfaceQuantityType, CountSurfaceQuantity>& SurfaceQuantityContainer::getCountQuantities() {
    return count_quantities_;
}

void SurfaceQuantityContainer::setArea(double area) {
    area_ = area;
}

double SurfaceQuantityContainer::getArea() const {
    return area_;
}

SurfaceQuantityContainer SurfaceQuantityContainerFactory::AllQuantities() {
    auto container = SurfaceQuantityContainer();
    container.addVectorQuantity(VectorSurfaceQuantity(VectorSurfaceQuantityType::IncidentEnergy));
    container.addVectorQuantity(VectorSurfaceQuantity(VectorSurfaceQuantityType::EntranceCosine));
    container.addCountQuantity(CountSurfaceQuantity(CountSurfaceQuantityType::NumberOfPhotons));
    return container;
}