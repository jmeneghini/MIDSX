#ifndef MIDSX_SURFACE_QUANTITY_H
#define MIDSX_SURFACE_QUANTITY_H

#include <memory>
#include "quantity.h"
#include <Eigen/Dense>
#include "photon.h"
#include "tally_data.h"

/**
 * @brief Enum class which represents the types of vector quantities which can be measured for a surface tally.
 */
enum class VectorSurfaceQuantityType {
    IncidentEnergy,
    EntranceCosine,
};


namespace VectorSurfaceQuantityHelper {
    std::string toString(VectorSurfaceQuantityType type);
}

/**
 * @brief Enum class which represents the types of count quantities which can be measured for a surface tally.
 */
enum class CountSurfaceQuantityType {
    NumberOfPhotons
};

namespace CountSurfaceQuantityHelper {
    std::string toString(CountSurfaceQuantityType type);
}

/**
 * @brief Class which represents a vector quantity for a surface tally.
 *
 * Essentially a wrapper around the VectorValue class but is specific to a measurement type
 */
class VectorSurfaceQuantity {
public:
    /**
     * @brief ValueExtractor is a function that extracts necessary values from a TempSurfaceTallyData object for a VectorSurfaceQuantity.
     *
     *
     * @param temp_surface_tally_data The TempSurfaceTallyData object to extract the value from.
     * @return The value extracted from the TempSurfaceTallyData object.
     */
    using ValueExtractor = std::function<double(const TempSurfaceTallyData&)>;

    /**
     * @brief Constructor for the VectorSurfaceQuantity class.
     *
     * @param type The type of the VectorSurfaceQuantity to be measured.
     */
    explicit VectorSurfaceQuantity(VectorSurfaceQuantityType type);

    /**
     * @brief Overloads the + operator for VectorSurfaceQuantity.
     *
     * @param other The VectorSurfaceQuantity to add to this VectorSurfaceQuantity.
     * @return The sum of the two VectorSurfaceQuantity objects.
     */

    VectorSurfaceQuantity operator+(const VectorSurfaceQuantity& other) const;

    /**
     * @brief Measures the VectorSurfaceQuantity for a TempSurfaceTallyData object.
     *
     * @param temp_surface_tally_data The TempSurfaceTallyData object to measure the VectorSurfaceQuantity for.
     */
    void measure(TempSurfaceTallyData& temp_surface_tally_data);

    /**
     * @brief Returns the type of the VectorSurfaceQuantity.
     *
     * @return The type of the VectorSurfaceQuantity.
     */
    VectorSurfaceQuantityType getType() const;

    // Wrapper functions for VectorValue. Self-explanatory.
    VectorValue getTotalValues();
    VectorValue getPrimaryValues() const;
    VectorValue getSingleIncoherentScatterValues() const;
    VectorValue getSingleCoherentScatterValues() const;
    VectorValue getMultipleScatterValues() const;
private:
    VectorSurfaceQuantityType type_;
    bool totaled_ = false;
    VectorValue total_values_;
    VectorValue primary_values_;
    VectorValue single_incoherent_scatter_values_;
    VectorValue single_coherent_scatter_values_;
    VectorValue multiple_scatter_values_;
    ValueExtractor valueExtractor_;
};

/**
 * @brief Class which represents a count quantity for a surface tally.
 *
 * Essentially a wrapper around the CountValue class but is specific to a measurement type
 */
class CountSurfaceQuantity {
public:
    /**
     * @brief ValueExtractor is a function that extracts necessary values from a TempSurfaceTallyData object for a CountSurfaceQuantity.
     *
     * @param temp_surface_tally_data The TempSurfaceTallyData object to extract the value from.
     * @return The value extracted from the TempSurfaceTallyData object.
     */
    using ValueExtractor = std::function<bool(const TempSurfaceTallyData&)>;

    /**
     * @brief Constructor for the CountSurfaceQuantity class.
     *
     * @param type The type of the CountSurfaceQuantity to be measured.
     */
    explicit CountSurfaceQuantity(CountSurfaceQuantityType type);

    /**
     * @brief Overloads the + operator for CountSurfaceQuantity.
     *
     * @param other The CountSurfaceQuantity to add to this CountSurfaceQuantity.
     * @return The sum of the two CountSurfaceQuantity objects.
     */
    CountSurfaceQuantity operator+(const CountSurfaceQuantity& other) const;

    /**
     * @brief Measures the CountSurfaceQuantity for a TempSurfaceTallyData object.
     *
     * @param temp_surface_tally_data The TempSurfaceTallyData object to measure the CountSurfaceQuantity for.
     */
    void measure(TempSurfaceTallyData& temp_surface_tally_data);

    /**
     * @brief Returns the type of the CountSurfaceQuantity.
     *
     * @return The type of the CountSurfaceQuantity.
     */
    CountSurfaceQuantityType getType() const;

    // Wrapper functions for CountValue. Self-explanatory.
    CountValue getTotalValues();
    CountValue getPrimaryValues() const;
    CountValue getSingleIncoherentScatterValues() const;
    CountValue getSingleCoherentScatterValues() const;
    CountValue getMultipleScatterValues() const;
private:
    CountSurfaceQuantityType type_;
    bool totaled_ = false;
    CountValue total_values_;
    CountValue primary_values_;
    CountValue single_incoherent_scatter_values_;
    CountValue single_coherent_scatter_values_;
    CountValue multiple_scatter_values_;
    ValueExtractor valueExtractor_;
};

#endif //MIDSX_SURFACE_QUANTITY_H
