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

VolumeQuantityContainer VolumeQuantityContainer::operator+(const VolumeQuantityContainer& other) const {
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
            throw std::runtime_error("Cannot add VolumeQuantityContainers with different vector quantities.");
        }
    }
    for (auto& count_quantity : count_quantities_) {
        if (other.count_quantities_.find(count_quantity.first) == other.count_quantities_.end()) {
            throw std::runtime_error("Cannot add VolumeQuantityContainers with different count quantities.");
        }
    }
    // check that the two containers have the same volume (e.g they are both the same volume tally)
    if (volume_ != other.volume_) {
        throw std::runtime_error("Cannot add VolumeQuantityContainers with different volumes.");
    }
    // add the quantities
    auto new_container = VolumeQuantityContainer();
    new_container.setVolume(volume_);
    for (auto& vector_quantity : vector_quantities_) {
        new_container.addVectorQuantity(vector_quantity.second + other.vector_quantities_.at(vector_quantity.first));
    }
    for (auto& count_quantity : count_quantities_) {
        new_container.addCountQuantity(count_quantity.second + other.count_quantities_.at(count_quantity.first));
    }
    return new_container;
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
