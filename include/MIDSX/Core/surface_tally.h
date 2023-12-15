#ifndef MIDSX_SURFACE_TALLY_H
#define MIDSX_SURFACE_TALLY_H

#include "tally_data.h"
#include "surface_quantity_container.h"


/**
 * @brief Virtual class which represents a surface tally.
 *
 * A geometrical surface which is used to measure quantities of interest upon a trigger (e.g. photon passing through).
 */
class SurfaceTally {
public:
    virtual ~SurfaceTally() = default;
    /**
     * @brief Processes the measurements for a TempSurfaceTallyData object.
     *
     * @param temp_surface_tally_data The TempSurfaceTallyData object to process the measurements for.
     */
    void processMeasurements(TempSurfaceTallyData& temp_surface_tally_data);

    /**
     * @brief Gets the SurfaceQuantityContainer object for the surface tally.
     *
     * @return The SurfaceQuantityContainer object for the surface tally.
     */
    SurfaceQuantityContainer& getSurfaceQuantityContainer();
protected:
    Eigen::Vector3d norm_;

    TempSurfaceTallyData temp_surface_tally_data_;
    SurfaceQuantityContainer surface_quantities_;

    bool willPassThrough();

    double calculateEntranceCosine();
    virtual double calculateIntersectionParameter(double entrance_cosine) = 0;
    Eigen::Vector3d calculateIntersectionPoint(double t);
    static bool isPhotonParallelToPlane(double entrance_cosine);
    bool isOutOfRange(double t);
    virtual bool isOutsideOfSurface(const Eigen::Vector3d& intersection_point) = 0;
};

/**
 * @brief Class which represents a disc surface tally.
 */
class DiscSurfaceTally : public SurfaceTally {
public:
    /**
     * @brief Constructor for the DiscSurfaceTally class.
     *
     * @param center The center of the disc.
     * @param norm The normal vector of the disc. Points in direction of desired traversal.
     * @param radius The radius of the disc.
     * @param quantities The SurfaceQuantityContainer object for the surface tally.
     */
    DiscSurfaceTally(Eigen::Vector3d center, Eigen::Vector3d norm, double radius, const SurfaceQuantityContainer& quantities);
private:
    Eigen::Vector3d center_;
    double radius_;

    void setArea();

    double calculateIntersectionParameter(double entrance_cosine) override;
    bool isOutsideOfSurface(const Eigen::Vector3d& intersection_point) override;
};

/**
 * @brief Class which represents a rectangular surface tally.
 */
class RectangularSurfaceTally : public SurfaceTally {
public:
    /**
     * @brief Constructor for the RectangularSurfaceTally class.
     *
     * @param corner The vector to the corner of the rectangular surface tally.
     * @param edge1 The vector along the first edge of the rectangular surface tally.
     * @param edge2 The vector along the second edge of the rectangular surface tally.
     * @param quantities The SurfaceQuantityContainer object for the surface tally.
     */
    RectangularSurfaceTally(Eigen::Vector3d corner, Eigen::Vector3d edge1, Eigen::Vector3d edge2, const SurfaceQuantityContainer& quantities);
private:
    Eigen::Vector3d corner_;
    Eigen::Vector3d edge1_;
    Eigen::Vector3d edge2_;
    double edge1_dot_edge1_;
    double edge2_dot_edge2_;

    void handleOrthogonalEdges();
    void setArea();
    void setBasisDotProducts();

    bool areEdgesOrthogonal();

    double calculateIntersectionParameter(double entrance_cosine) override;
    bool isOutsideOfSurface(const Eigen::Vector3d& intersection_point) override;
};

#endif //MIDSX_SURFACE_TALLY_H
