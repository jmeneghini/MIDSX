#include "Core/surface_quantity.h"

namespace {
    std::unordered_map<VectorSurfaceQuantityType, std::string> vectorTypeToString = {
            {VectorSurfaceQuantityType::IncidentEnergy, "IncidentEnergy"},
            {VectorSurfaceQuantityType::EntranceCosine, "EntranceCosine"}
    };

    std::unordered_map<CountSurfaceQuantityType, std::string> countTypeToString = {
            {CountSurfaceQuantityType::NumberOfPhotons, "NumberOfPhotons"}
    };
}

std::string VectorSurfaceQuantityHelper::toString(VectorSurfaceQuantityType type) {
    return vectorTypeToString[type];
}

std::string CountSurfaceQuantityHelper::toString(CountSurfaceQuantityType type) {
    return countTypeToString[type];
}

VectorSurfaceQuantity::VectorSurfaceQuantity(VectorSurfaceQuantityType type) {
    type_ = type;
    if (type == VectorSurfaceQuantityType::IncidentEnergy) {
        valueExtractor_ = [](const TempSurfaceTallyData& temp_surface_tally_data) {
            return temp_surface_tally_data.initial_photon.getEnergy();
        };
    } else if (type == VectorSurfaceQuantityType::EntranceCosine) {
        valueExtractor_ = [](const TempSurfaceTallyData& temp_surface_tally_data) {
            return temp_surface_tally_data.entrance_cosine;
        };
    } else {
        throw std::runtime_error("Unknown VectorSurfaceQuantityType");
    }
}

VectorSurfaceQuantity VectorSurfaceQuantity::operator+(VectorSurfaceQuantity& other) const {
    if (type_ != other.type_) {
        throw std::runtime_error("Cannot add VectorSurfaceQuantity objects of different types");
    }
    VectorSurfaceQuantity new_vector_surface_quantity(type_);
    new_vector_surface_quantity.primary_values_ = primary_values_ + other.primary_values_;
    new_vector_surface_quantity.single_incoherent_scatter_values_ = single_incoherent_scatter_values_ + other.single_incoherent_scatter_values_;
    new_vector_surface_quantity.single_coherent_scatter_values_ = single_coherent_scatter_values_ + other.single_coherent_scatter_values_;
    new_vector_surface_quantity.multiple_scatter_values_ = multiple_scatter_values_ + other.multiple_scatter_values_;
    return new_vector_surface_quantity;
}

void VectorSurfaceQuantity::measure(TempSurfaceTallyData &temp_surface_tally_data) {
double value = valueExtractor_(temp_surface_tally_data);
    if (value == 0) {
        return;
    }
    if (temp_surface_tally_data.initial_photon.getTotalScatterCount() > 1) {
        multiple_scatter_values_.addValue(value);
    } else if (temp_surface_tally_data.initial_photon.getCoherentScatterCount() == 1) {
        single_coherent_scatter_values_.addValue(value);
    } else if (temp_surface_tally_data.initial_photon.getIncoherentScatterCount() == 1) {
        single_incoherent_scatter_values_.addValue(value);
    } else {
        primary_values_.addValue(value);
    }
}

VectorSurfaceQuantityType VectorSurfaceQuantity::getType() const {
    return type_;
}

VectorValue& VectorSurfaceQuantity::getTotalValues() {
    if (!totaled_) {
        total_values_.addValues(primary_values_.getVector());
        total_values_.addValues(single_incoherent_scatter_values_.getVector());
        total_values_.addValues(single_coherent_scatter_values_.getVector());
        total_values_.addValues(multiple_scatter_values_.getVector());
        totaled_ = true;
    }
    return total_values_;
}

VectorValue& VectorSurfaceQuantity::getPrimaryValues() {
    return primary_values_;
}

VectorValue& VectorSurfaceQuantity::getSingleIncoherentScatterValues() {
    return single_incoherent_scatter_values_;
}

VectorValue& VectorSurfaceQuantity::getSingleCoherentScatterValues() {
    return single_coherent_scatter_values_;
}

VectorValue& VectorSurfaceQuantity::getMultipleScatterValues() {
    return multiple_scatter_values_;
}

CountSurfaceQuantity::CountSurfaceQuantity(CountSurfaceQuantityType type) {
    type_ = type;
    if (type == CountSurfaceQuantityType::NumberOfPhotons) {
        valueExtractor_ = [](const TempSurfaceTallyData& temp_surface_tally_data) {
            if (temp_surface_tally_data.already_counted) {
                return false;
            }
            return true;
        };
    } else {
        throw std::runtime_error("Unknown CountSurfaceQuantityType");
    }
}

CountSurfaceQuantity CountSurfaceQuantity::operator+(const CountSurfaceQuantity& other) const {
    if (type_ != other.type_) {
        throw std::runtime_error("Cannot add CountSurfaceQuantity objects of different types");
    }
    CountSurfaceQuantity new_count_surface_quantity(type_);
    new_count_surface_quantity.primary_values_ = primary_values_ + other.primary_values_;
    new_count_surface_quantity.single_incoherent_scatter_values_ = single_incoherent_scatter_values_ + other.single_incoherent_scatter_values_;
    new_count_surface_quantity.single_coherent_scatter_values_ = single_coherent_scatter_values_ + other.single_coherent_scatter_values_;
    new_count_surface_quantity.multiple_scatter_values_ = multiple_scatter_values_ + other.multiple_scatter_values_;
    return new_count_surface_quantity;
}

void CountSurfaceQuantity::measure(TempSurfaceTallyData &temp_surface_tally_data) {
    bool doCount = valueExtractor_(temp_surface_tally_data);
    if (!doCount) {
        return;
    }
    if (temp_surface_tally_data.initial_photon.getTotalScatterCount() > 1) {
        multiple_scatter_values_.addCount();
    } else if (temp_surface_tally_data.initial_photon.getCoherentScatterCount() == 1) {
        single_coherent_scatter_values_.addCount();
    } else if (temp_surface_tally_data.initial_photon.getIncoherentScatterCount() == 1) {
        single_incoherent_scatter_values_.addCount();
    } else {
        primary_values_.addCount();
    }
}

CountSurfaceQuantityType CountSurfaceQuantity::getType() const {
    return type_;
}

CountValue CountSurfaceQuantity::getTotalValues() {
    if (!totaled_) {
        total_values_.addCounts(primary_values_.getCount());
        total_values_.addCounts(single_incoherent_scatter_values_.getCount());
        total_values_.addCounts(single_coherent_scatter_values_.getCount());
        total_values_.addCounts(multiple_scatter_values_.getCount());
        totaled_ = true;
    }
    return total_values_;
}

CountValue CountSurfaceQuantity::getPrimaryValues() const {
    return primary_values_;
}

CountValue CountSurfaceQuantity::getSingleIncoherentScatterValues() const {
    return single_incoherent_scatter_values_;
}

CountValue CountSurfaceQuantity::getSingleCoherentScatterValues() const {
    return single_coherent_scatter_values_;
}

CountValue CountSurfaceQuantity::getMultipleScatterValues() const {
    return multiple_scatter_values_;
}
