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

void SurfaceQuantityContainer::merge(const std::vector<SurfaceQuantityContainer>& containers) {
    for (auto& container : containers) {
        for (auto& vector_quantity : container.vector_quantities_) {
            if (vector_quantities_.find(vector_quantity.first) == vector_quantities_.end()) {
                vector_quantities_.emplace(vector_quantity.first, vector_quantity.second);
            }
        }
        for (auto& count_quantity : container.count_quantities_) {
            if (count_quantities_.find(count_quantity.first) == count_quantities_.end()) {
                count_quantities_.emplace(count_quantity.first, count_quantity.second);
            }
        }
    }
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