#ifndef MIDSX_VOLUME_QUANTITY_H
#define MIDSX_VOLUME_QUANTITY_H

#include <memory>
#include <string>
#include <vector>
#include "tally_data.h"
#include "quantity.h"

/**
 * @brief Enum class which represents the types of vector quantities which can be measured for a volume tally.
 */
enum class VectorVolumeQuantityType {
    EnergyDeposition,
    IncidentEnergy
};

namespace VectorVolumeQuantityHelper {
    std::string toString(VectorVolumeQuantityType type);
}

/**
 * @brief Enum class which represents the types of count quantities which can be measured for a volume tally.
 */
enum class CountVolumeQuantityType {
    NumberOfPhotons,
    NumberOfInteractions
};

namespace CountVolumeQuantityHelper {
    std::string toString(CountVolumeQuantityType type);
}

/**
 * @brief Class which represents a vector quantity for a volume tally.
 *
 * Essentially a wrapper around the VectorValue class but is specific to a measurement type
 */
class VectorVolumeQuantity {
public:
    /**
     * @brief ValueExtractor is a function that extracts necessary values from a TempVolumeTallyData object for a VectorVolumeQuantity.
     *
     * @param temp_volume_tally_data The TempVolumeTallyData object to extract the value from.
     * @return The value extracted from the TempVolumeTallyData object.
     */
    using ValueExtractor = std::function<double(const TempVolumeTallyData&)>;

    /**
     * @brief Constructor for the VectorVolumeQuantity class.
     *
     * @param type The type of the VectorVolumeQuantity to be measured.
     */
    explicit VectorVolumeQuantity(VectorVolumeQuantityType type);

    /**
     * @brief Overloads the + operator for VectorVolumeQuantity.
     *
     * @param other The VectorVolumeQuantity to add to this VectorVolumeQuantity.
     * @return The sum of the two VectorVolumeQuantity objects.
     */
    VectorVolumeQuantity operator+(const VectorVolumeQuantity& other) const;

    /**
     * @brief Measures the VectorVolumeQuantity for a TempVolumeTallyData object.
     *
     * @param temp_volume_tally_data The TempVolumeTallyData object to measure the VectorVolumeQuantity for.
     */
    void measure(TempVolumeTallyData& temp_volume_tally_data);

    /**
     * @brief Returns the type of the VectorVolumeQuantity.
     *
     * @return The type of the VectorVolumeQuantity.
     */
    VectorVolumeQuantityType getType() const;
    VectorValue getTotalValues();
    VectorValue getPrimaryValues() const;
    VectorValue getSingleIncoherentScatterValues() const;
    VectorValue getSingleCoherentScatterValues() const;
    VectorValue getMultipleScatterValues() const;
private:
    VectorVolumeQuantityType type_;
    bool totaled_ = false;
    VectorValue total_values_;
    VectorValue primary_values_;
    VectorValue single_incoherent_scatter_values_;
    VectorValue single_coherent_scatter_values_;
    VectorValue multiple_scatter_values_;
    ValueExtractor valueExtractor_;
};

class CountVolumeQuantity {
public:
    /**
     * @brief ValueExtractor is a function that extracts necessary values from a TempVolumeTallyData object for a CountVolumeQuantity.
     *
     * @param temp_volume_tally_data The TempVolumeTallyData object to extract the value from.
     *
     * @return The value extracted from the TempVolumeTallyData object.
     */
    using ValueExtractor = std::function<bool(const TempVolumeTallyData&)>;

    /**
     * @brief Constructor for the CountVolumeQuantity class.
     *
     * @param type The type of the CountVolumeQuantity to be measured.
     */
    explicit CountVolumeQuantity(CountVolumeQuantityType type);

    /**
     * @brief Overloads the + operator for CountVolumeQuantity.
     *
     * @param other The CountVolumeQuantity to add to this CountVolumeQuantity.
     * @return The sum of the two CountVolumeQuantity objects.
     */
    CountVolumeQuantity operator+(const CountVolumeQuantity& other) const;

    /**
     * @brief Measures the CountVolumeQuantity for a TempVolumeTallyData object.
     *
     * @param temp_volume_tally_data The TempVolumeTallyData object to measure the CountVolumeQuantity for.
     */
    void measure(TempVolumeTallyData& temp_volume_tally_data);

    /**
     * @brief Returns the type of the CountVolumeQuantity.
     *
     * @return The type of the CountVolumeQuantity.
     */
    CountVolumeQuantityType getType() const;
    CountValue getTotalValues();
    CountValue getPrimaryValues() const;
    CountValue getSingleIncoherentScatterValues() const;
    CountValue getSingleCoherentScatterValues() const;
    CountValue getMultipleScatterValues() const;
private:
    CountVolumeQuantityType type_;
    bool totaled_ = false;
    CountValue total_values_;
    CountValue primary_values_;
    CountValue single_incoherent_scatter_values_;
    CountValue single_coherent_scatter_values_;
    CountValue multiple_scatter_values_;
    ValueExtractor valueExtractor_;
};

#endif //MIDSX_VOLUME_QUANTITY_H
