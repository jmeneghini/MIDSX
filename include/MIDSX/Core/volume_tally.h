#ifndef MIDSX_VOLUME_TALLY_CPP_H
#define MIDSX_VOLUME_TALLY_CPP_H

#include "tally_data.h"
#include "volume_quantity_container.h"

enum class VolumeTraversal {
    MISSES,
    STARTS_INSIDE_EXITS,
    STARTS_INSIDE_STAYS,
    PASSES_THROUGH,
    LANDS_INSIDE
};

class VolumeTally {
public:
    virtual ~VolumeTally() = default;
    void processMeasurements(TempVolumeTallyData& temp_volume_tally_data);
    VolumeQuantityContainer& getVolumeQuantityContainer();
protected:
    virtual VolumeTraversal determineVolumeTraversal() = 0;
    TempVolumeTallyData temp_volume_tally_data_;
    VolumeQuantityContainer quantities_;
    // need volume quantity container
};

class AACuboidVolumeTally : public VolumeTally {
public:
    AACuboidVolumeTally(Eigen::Vector3d  min_corner, Eigen::Vector3d  max_corner, const VolumeQuantityContainer& quantities);
private:
    Eigen::Vector3d min_corner_;
    Eigen::Vector3d max_corner_;

    VolumeTraversal determineVolumeTraversal() override;

    std::pair<double, double> getEnteringAndExitingLengths();

    std::pair<Eigen::Vector3d, Eigen::Vector3d> getLengthsToSurfacePlanes();
};

#endif //MIDSX_VOLUME_TALLY_CPP_H
