#ifndef MIDSX_VOLUME_QUANTITY_CONTAINER_H
#define MIDSX_VOLUME_QUANTITY_CONTAINER_H

#include <memory>
#include <unordered_map>
#include "volume_quantity.h"
#include "tally_data.h"

class VolumeQuantityContainer {
public:
    void addVectorQuantity(const VectorVolumeQuantity& quantity);
    void addCountQuantity(const CountVolumeQuantity& quantity);
    void measureAll(TempVolumeTallyData& temp_tally_data);
    void merge(const std::vector<VolumeQuantityContainer>& containers);
    std::unordered_map<VectorVolumeQuantityType, VectorVolumeQuantity>& getVectorQuantities();
    std::unordered_map<CountVolumeQuantityType, CountVolumeQuantity>& getCountQuantities();

    void setVolume(double volume);
    double getVolume() const;
private:
    std::unordered_map<VectorVolumeQuantityType, VectorVolumeQuantity> vector_quantities_;
    std::unordered_map<CountVolumeQuantityType, CountVolumeQuantity> count_quantities_;
    double volume_;
};

namespace VolumeQuantityContainerFactory {
    VolumeQuantityContainer AllQuantities();
    VolumeQuantityContainer EnergyDeposition();
}

#endif //MIDSX_VOLUME_QUANTITY_CONTAINER_H
