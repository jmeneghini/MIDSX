#ifndef MIDSX_VOLUME_TALLY_CPP_H
#define MIDSX_VOLUME_TALLY_CPP_H

#include "tally_data.h"
#include "volume_quantity_container.h"

/**
 * @brief Enum class which represents the types of volume traversals which can be measured for a volume tally.
 */
enum class VolumeTraversal {
    MISSES,
    STARTS_INSIDE_EXITS,
    STARTS_INSIDE_STAYS,
    PASSES_THROUGH,
    LANDS_INSIDE
};

/**
 * @brief Virtual class which represents a volume tally.
 *
 * A geometrical volume which is used to measure quantities of interest upon a trigger (e.g. photon passing through).
 */
class VolumeTally {
public:
    virtual ~VolumeTally() = default;

    /**
     * @brief Processes the measurements for a TempVolumeTallyData object.
     *
     * @param temp_volume_tally_data The TempVolumeTallyData object to process the measurements for.
     */
    void processMeasurements(TempVolumeTallyData& temp_volume_tally_data);

    /**
     * @brief Gets the VolumeQuantityContainer object for the volume tally.
     *
     * @return The VolumeQuantityContainer object for the volume tally.
     */
    VolumeQuantityContainer& getVolumeQuantityContainer();
protected:
    virtual VolumeTraversal determineVolumeTraversal() = 0;
    TempVolumeTallyData temp_volume_tally_data_;
    VolumeQuantityContainer quantities_;
    // need volume quantity container
};

/**
 * @brief Class which represents an axis-aligned cuboid volume tally.
 */
class AACuboidVolumeTally : public VolumeTally {
public:
    /**
     * @brief Constructor for the AACuboidVolumeTally class.
     *
     * @param min_corner The vector from origin to the closest corner of the cuboid.
     * @param max_corner The vector from origin to the furthest corner of the cuboid.
     * @param quantities The VolumeQuantityContainer object for the volume tally.
     */
    AACuboidVolumeTally(Eigen::Vector3d  min_corner, Eigen::Vector3d  max_corner, const VolumeQuantityContainer& quantities);
private:
    Eigen::Vector3d min_corner_;
    Eigen::Vector3d max_corner_;

    VolumeTraversal determineVolumeTraversal() override;

    std::pair<double, double> getEnteringAndExitingLengths();

    std::pair<Eigen::Vector3d, Eigen::Vector3d> getLengthsToSurfacePlanes();
};

#endif //MIDSX_VOLUME_TALLY_CPP_H
