#ifndef MIDSX_VOLUME_QUANTITY_CONTAINER_H
#define MIDSX_VOLUME_QUANTITY_CONTAINER_H

#include <memory>
#include <unordered_map>
#include "volume_quantity.h"
#include "tally_data.h"

/**
 * @brief Class which represents a container for volume quantities.
 *
 * Essentially a wrapper around a map of VolumeQuantityType to their respective VolumeQuantity objects.
 * Allows users to specify which quantities they want to measure for a volume tally.
 */
class VolumeQuantityContainer {
public:
    /**
     * @brief Adds a vector quantity to the container.
     *
     * @param quantity The vector quantity to add to the container.
     */
    void addVectorQuantity(const VectorVolumeQuantity& quantity);

    /**
     * @brief Adds a count quantity to the container.
     *
     * @param quantity The count quantity to add to the container.
     */
    void addCountQuantity(const CountVolumeQuantity& quantity);

    /**
     * @brief Measures all quantities in the container for a TempVolumeTallyData object.
     *
     * @param temp_tally_data The TempVolumeTallyData object to measure the quantities for.
     */
    void measureAll(TempVolumeTallyData& temp_tally_data);

    /**
     * @brief Overloads the + operator for VolumeQuantityContainer.
     *
     * @param other The VolumeQuantityContainer to add to this VolumeQuantityContainer.
     * @return The sum of the two VolumeQuantityContainer objects.
     */
    VolumeQuantityContainer operator+(VolumeQuantityContainer& other) const;

    /**
     * @brief Clears the container of all quantities.
     *
     */
    void clear();

    /**
     * @brief Gets the map of vector quantities in the container.
     *
     * @return The map of vector quantities in the container.
     */
    std::unordered_map<VectorVolumeQuantityType, VectorVolumeQuantity>& getVectorQuantities();

    /**
     * @brief Gets the map of count quantities in the container.
     *
     * @return The map of count quantities in the container.
     */
    std::unordered_map<CountVolumeQuantityType, CountVolumeQuantity>& getCountQuantities();

    /**
     * @brief Sets the volume of the volume tally.
     *
     * @param volume The volume of the volume tally.
     */
    void setVolume(double volume);

    /**
     * @brief Gets the volume of the volume tally.
     *
     * @return The volume of the volume tally.
     */
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
