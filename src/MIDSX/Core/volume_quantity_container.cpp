#include "Core/volume_quantity_container.h"

void VolumeQuantityContainer::addVectorQuantity(const VectorVolumeQuantity& quantity) {
    vector_quantities_.emplace(quantity.getType(), quantity);
}

void VolumeQuantityContainer::addCountQuantity(const CountVolumeQuantity& quantity) {
    count_quantities_.emplace(quantity.getType(), quantity);
}

void VolumeQuantityContainer::measureAll(TempVolumeTallyData& temp_tally_data) {
    for (auto& vector_quantity : vector_quantities_) {
        vector_quantity.second.measure(temp_tally_data);
    }
    for (auto& count_quantity : count_quantities_) {
        count_quantity.second.measure(temp_tally_data);
    }
}

void VolumeQuantityContainer::merge(const std::vector<VolumeQuantityContainer> &containers) {
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

std::unordered_map<VectorVolumeQuantityType, VectorVolumeQuantity>& VolumeQuantityContainer::getVectorQuantities() {
    return vector_quantities_;
}

std::unordered_map<CountVolumeQuantityType, CountVolumeQuantity>& VolumeQuantityContainer::getCountQuantities() {
    return count_quantities_;
}

void VolumeQuantityContainer::setVolume(double volume) {
    volume_ = volume;
}

double VolumeQuantityContainer::getVolume() const {
    return volume_;
}

VolumeQuantityContainer VolumeQuantityContainerFactory::AllQuantities() {
    auto container = VolumeQuantityContainer();
    container.addVectorQuantity(VectorVolumeQuantity(VectorVolumeQuantityType::EnergyDeposition));
    container.addVectorQuantity(VectorVolumeQuantity(VectorVolumeQuantityType::IncidentEnergy));
    container.addCountQuantity(CountVolumeQuantity(CountVolumeQuantityType::NumberOfPhotons));
    container.addCountQuantity(CountVolumeQuantity(CountVolumeQuantityType::NumberOfInteractions));

    return container;
}
