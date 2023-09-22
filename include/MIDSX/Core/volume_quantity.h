#ifndef MIDSX_VOLUME_QUANTITY_H
#define MIDSX_VOLUME_QUANTITY_H

#include <memory>
#include <string>
#include <vector>
#include "tally_data.h"
#include "quantity.h"

enum class VectorVolumeQuantityType {
    EnergyDeposition,
    IncidentEnergy
};

namespace VectorVolumeQuantityHelper {
    std::string toString(VectorVolumeQuantityType type);
}

enum class CountVolumeQuantityType {
    NumberOfPhotons,
    NumberOfInteractions
};

namespace CountVolumeQuantityHelper {
    std::string toString(CountVolumeQuantityType type);
}

class VectorVolumeQuantity {
public:
    using ValueExtractor = std::function<double(const TempVolumeTallyData&)>;

    explicit VectorVolumeQuantity(VectorVolumeQuantityType type);
    void measure(TempVolumeTallyData& temp_volume_tally_data);
    VectorVolumeQuantityType getType() const;
    VectorValue getPrimaryValues() const;
    VectorValue getSingleIncoherentScatterValues() const;
    VectorValue getSingleCoherentScatterValues() const;
    VectorValue getMultipleScatterValues() const;
private:
    VectorVolumeQuantityType type_;
    VectorValue primary_values_;
    VectorValue single_incoherent_scatter_values_;
    VectorValue single_coherent_scatter_values_;
    VectorValue multiple_scatter_values_;
    ValueExtractor valueExtractor_;
};

class CountVolumeQuantity {
public:
    using ValueExtractor = std::function<bool(const TempVolumeTallyData&)>;

    explicit CountVolumeQuantity(CountVolumeQuantityType type);
    void measure(TempVolumeTallyData& temp_volume_tally_data);
    CountVolumeQuantityType getType() const;
    CountValue getPrimaryValues() const;
    CountValue getSingleIncoherentScatterValues() const;
    CountValue getSingleCoherentScatterValues() const;
    CountValue getMultipleScatterValues() const;
private:
    CountVolumeQuantityType type_;
    CountValue primary_values_;
    CountValue single_incoherent_scatter_values_;
    CountValue single_coherent_scatter_values_;
    CountValue multiple_scatter_values_;
    ValueExtractor valueExtractor_;
};

#endif //MIDSX_VOLUME_QUANTITY_H
