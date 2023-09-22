#include "Core/volume_quantity.h"

namespace {
    std::unordered_map<VectorVolumeQuantityType, std::string> vectorTypeToString = {
            {VectorVolumeQuantityType::EnergyDeposition, "EnergyDeposition"},
            {VectorVolumeQuantityType::IncidentEnergy, "IncidentEnergy"}
    };

    std::unordered_map<CountVolumeQuantityType, std::string> countTypeToString = {
            {CountVolumeQuantityType::NumberOfPhotons, "NumberOfPhotons"},
            {CountVolumeQuantityType::NumberOfInteractions, "NumberOfInteractions"}
    };
}

std::string VectorVolumeQuantityHelper::toString(VectorVolumeQuantityType type) {
    return vectorTypeToString[type];
}

std::string CountVolumeQuantityHelper::toString(CountVolumeQuantityType type) {
    return countTypeToString[type];
}

VectorVolumeQuantity::VectorVolumeQuantity(VectorVolumeQuantityType type) {
    type_ = type;
    if (type == VectorVolumeQuantityType::EnergyDeposition) {
        valueExtractor_ = [](const TempVolumeTallyData& temp_volume_tally_data) {
            return temp_volume_tally_data.energy_deposited;
        };
    } else if (type == VectorVolumeQuantityType::IncidentEnergy) {
        valueExtractor_ = [](const TempVolumeTallyData& temp_volume_tally_data) {
            if (!temp_volume_tally_data.already_counted) {
                return temp_volume_tally_data.initial_photon.getEnergy();
            }
                return 0.0;
        };
    } else {
        throw std::runtime_error("Unknown VectorVolumeQuantityType");
    }
}


void VectorVolumeQuantity::measure(TempVolumeTallyData &temp_volume_tally_data) {
    double value = valueExtractor_(temp_volume_tally_data);
    if (value == 0) {
        return;
    }
    if (temp_volume_tally_data.final_photon.getTotalScatterCount() > 1) {
        multiple_scatter_values_.addValue(value);
    } else if (temp_volume_tally_data.final_photon.getCoherentScatterCount() == 1) {
        single_coherent_scatter_values_.addValue(value);
    } else if (temp_volume_tally_data.final_photon.getIncoherentScatterCount() == 1) {
        single_incoherent_scatter_values_.addValue(value);
    } else {
        primary_values_.addValue(value);
    }
}

VectorVolumeQuantityType VectorVolumeQuantity::getType() const {
    return type_;
}

VectorValue VectorVolumeQuantity::getPrimaryValues() const {
    return primary_values_;
}

VectorValue VectorVolumeQuantity::getSingleIncoherentScatterValues() const {
    return single_incoherent_scatter_values_;
}

VectorValue VectorVolumeQuantity::getSingleCoherentScatterValues() const {
    return single_coherent_scatter_values_;
}

VectorValue VectorVolumeQuantity::getMultipleScatterValues() const {
    return multiple_scatter_values_;
}

CountVolumeQuantity::CountVolumeQuantity(CountVolumeQuantityType type) {
    type_ = type;
    if (type == CountVolumeQuantityType::NumberOfPhotons) {
        valueExtractor_ = [](const TempVolumeTallyData& temp_volume_tally_data) {
            if (temp_volume_tally_data.already_counted) {
                return false;
            }
            return true;
        };
    } else if (type == CountVolumeQuantityType::NumberOfInteractions) {
        valueExtractor_ = [](const TempVolumeTallyData& temp_volume_tally_data) {
            return temp_volume_tally_data.isInteract;
        };
    } else {
        throw std::runtime_error("Unknown CountVolumeQuantityType");
    }
}

void CountVolumeQuantity::measure(TempVolumeTallyData &temp_volume_tally_data) {
    bool doCount = valueExtractor_(temp_volume_tally_data);
    if (!doCount) {
        return;
    }
    if (temp_volume_tally_data.final_photon.getTotalScatterCount() > 1) {
        multiple_scatter_values_.addCount();
    } else if (temp_volume_tally_data.final_photon.getCoherentScatterCount() == 1) {
        single_coherent_scatter_values_.addCount();
    } else if (temp_volume_tally_data.final_photon.getIncoherentScatterCount() == 1) {
        single_incoherent_scatter_values_.addCount();
    } else {
        primary_values_.addCount();
    }
}

CountVolumeQuantityType CountVolumeQuantity::getType() const {
    return type_;
}

CountValue CountVolumeQuantity::getPrimaryValues() const {
    return primary_values_;
}

CountValue CountVolumeQuantity::getSingleIncoherentScatterValues() const {
    return single_incoherent_scatter_values_;
}

CountValue CountVolumeQuantity::getSingleCoherentScatterValues() const {
    return single_coherent_scatter_values_;
}

CountValue CountVolumeQuantity::getMultipleScatterValues() const {
    return multiple_scatter_values_;
}
