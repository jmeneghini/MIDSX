#ifndef MIDSX_SURFACE_QUANTITY_CONTAINER_H
#define MIDSX_SURFACE_QUANTITY_CONTAINER_H

#include <memory>
#include <unordered_map>
#include "surface_quantity.h"

class SurfaceQuantityContainer {
public:
    void addVectorQuantity(const VectorSurfaceQuantity& quantity);
    void addCountQuantity(const CountSurfaceQuantity& quantity);
    void measureAll(TempSurfaceTallyData& temp_tally_data);
    void merge(const std::vector<SurfaceQuantityContainer>& containers);
    std::unordered_map<VectorSurfaceQuantityType, VectorSurfaceQuantity>& getVectorQuantities();
    std::unordered_map<CountSurfaceQuantityType, CountSurfaceQuantity>& getCountQuantities();

    void setArea(double area);
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
