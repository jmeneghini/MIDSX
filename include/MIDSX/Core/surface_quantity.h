#ifndef MIDSX_SURFACE_QUANTITY_H
#define MIDSX_SURFACE_QUANTITY_H

#include <memory>
#include "quantity.h"

enum class VectorSurfaceQuantityType {
    IncidentEnergy,
    EntranceCosine,
};


namespace VectorSurfaceQuantityHelper {
    std::string toString(VectorSurfaceQuantityType type);
}

enum class CountSurfaceQuantityType {
    NumberOfPhotons
};

namespace CountSurfaceQuantityHelper {
    std::string toString(CountSurfaceQuantityType type);
}

class VectorSurfaceQuantity {
public:
    using ValueExtractor = std::function<double(const TempSurfaceTallyData&)>;
    explicit VectorSurfaceQuantity(VectorSurfaceQuantityType type);
    void measure(TempSurfaceTallyData& temp_surface_tally_data);
    VectorSurfaceQuantityType getType() const;
    VectorValue getPrimaryValues() const;
    VectorValue getSingleIncoherentScatterValues() const;
    VectorValue getSingleCoherentScatterValues() const;
    VectorValue getMultipleScatterValues() const;
private:
    VectorSurfaceQuantityType type_;
    VectorValue primary_values_;
    VectorValue single_incoherent_scatter_values_;
    VectorValue single_coherent_scatter_values_;
    VectorValue multiple_scatter_values_;
    ValueExtractor valueExtractor_;
};

class CountSurfaceQuantity {
public:
    using ValueExtractor = std::function<bool(const TempSurfaceTallyData&)>;
    explicit CountSurfaceQuantity(CountSurfaceQuantityType type);
    void measure(TempSurfaceTallyData& temp_surface_tally_data);
    CountSurfaceQuantityType getType() const;
    CountValue getPrimaryValues() const;
    CountValue getSingleIncoherentScatterValues() const;
    CountValue getSingleCoherentScatterValues() const;
    CountValue getMultipleScatterValues() const;
private:
    CountSurfaceQuantityType type_;
    CountValue primary_values_;
    CountValue single_incoherent_scatter_values_;
    CountValue single_coherent_scatter_values_;
    CountValue multiple_scatter_values_;
    ValueExtractor valueExtractor_;
};

#endif //MIDSX_SURFACE_QUANTITY_H
