#ifndef MIDSX_SURFACE_QUANTITY_CONTAINER_H
#define MIDSX_SURFACE_QUANTITY_CONTAINER_H

#include <memory>
#include <unordered_map>
#include "surface_quantity.h"

/**
 * @brief Class which represents a container for surface quantities.
 *
 * Essentially a wrapper around a map of SurfaceQuantityType to their respective SurfaceQuantity objects.
 * Allows users to specify which quantities they want to measure for a surface tally.
 */
class SurfaceQuantityContainer {
public:
    /**
     * @brief Adds a vector quantity to the container.
     *
     * @param quantity The vector quantity to add to the container.
     */
    void addVectorQuantity(const VectorSurfaceQuantity& quantity);

    /**
     * @brief Adds a count quantity to the container.
     *
     * @param quantity The count quantity to add to the container.
     */
    void addCountQuantity(const CountSurfaceQuantity& quantity);

    /**
     * @brief Measures all quantities in the container for a TempSurfaceTallyData object.
     *
     * @param temp_tally_data The TempSurfaceTallyData object to measure the quantities for.
     */
    void measureAll(TempSurfaceTallyData& temp_tally_data);

    /**
     * @brief Overloads the + operator for SurfaceQuantityContainer.
     *
     * @param other The SurfaceQuantityContainer to add to this SurfaceQuantityContainer.
     * @return The sum of the two SurfaceQuantityContainer objects.
     */
    SurfaceQuantityContainer operator+(const SurfaceQuantityContainer& other) const;

    /**
     * @brief Gets the map of vector quantities in the container.
     *
     * @return The map of vector quantities in the container.
     */
    std::unordered_map<VectorSurfaceQuantityType, VectorSurfaceQuantity>& getVectorQuantities();

    /**
     * @brief Gets the map of count quantities in the container.
     *
     * @return The map of count quantities in the container.
     */
    std::unordered_map<CountSurfaceQuantityType, CountSurfaceQuantity>& getCountQuantities();

    /**
     * @brief Sets the area of the surface tally.
     *
     * @param area The area of the surface tally.
     */
    void setArea(double area);

    /**
     * @brief Gets the area of the surface tally.
     *
     * @return The area of the surface tally.
     */
    double getArea() const;
private:
    std::unordered_map<VectorSurfaceQuantityType, VectorSurfaceQuantity> vector_quantities_;
    std::unordered_map<CountSurfaceQuantityType, CountSurfaceQuantity> count_quantities_;
    double area_;
};

namespace SurfaceQuantityContainerFactory {
    SurfaceQuantityContainer AllQuantities();
}

#endif //MIDSX_SURFACE_QUANTITY_CONTAINER_H
