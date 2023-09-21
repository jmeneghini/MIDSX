#ifndef MIDSX_SURFACE_TALLY_H
#define MIDSX_SURFACE_TALLY_H

#include "tally_data.h"
#include "surface_quantity_container.h"

class SurfaceTally {
public:
    virtual ~SurfaceTally() = default;
    void processMeasurements(TempSurfaceTallyData& temp_surface_tally_data);
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

class DiscSurfaceTally : public SurfaceTally {
public:
    DiscSurfaceTally(Eigen::Vector3d center, Eigen::Vector3d norm, double radius, const SurfaceQuantityContainer& quantities);
private:
    Eigen::Vector3d center_;
    double radius_;

    void setArea();

    double calculateIntersectionParameter(double entrance_cosine) override;
    bool isOutsideOfSurface(const Eigen::Vector3d& intersection_point) override;
};

class RectangularSurfaceTally : public SurfaceTally {
public:
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
