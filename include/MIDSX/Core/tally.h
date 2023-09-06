#ifndef MCXRAYTRANSPORT_TALLY_H
#define MCXRAYTRANSPORT_TALLY_H

#include "quantity.h"
#include "quantity_container.h"
#include "photon.h"
#include <Eigen/Dense>
#include <utility>

class Tally {
public:
    virtual void processMeasurements(TempTallyData& temp_tally_data) = 0;
    std::unique_ptr<QuantityContainer> getQuantityContainer();
protected:
    TempTallyData temp_tally_data_;
    std::unique_ptr<QuantityContainer> quantities_;
    double EPSILON_ = 1E-9;
};

enum class VolumeTraversal {
    MISSES,
    STARTS_INSIDE_EXITS,
    STARTS_INSIDE_STAYS,
    PASSES_THROUGH,
    LANDS_INSIDE
};

class VolumeTally : public Tally {
protected:
    virtual VolumeTraversal determineVolumeTraversal() = 0;
};

class AACuboidVolumeTally : public VolumeTally {
public:
    AACuboidVolumeTally(Eigen::Vector3d min_corner, Eigen::Vector3d max_corner, std::unique_ptr<QuantityContainer> quantities);
    void processMeasurements(TempTallyData& temp_tally_data) override;
private:
    Eigen::Vector3d min_corner_;
    Eigen::Vector3d max_corner_;

    VolumeTraversal determineVolumeTraversal() override;

    std::pair<double, double> getEnteringAndExitingLengths();

    std::pair<Eigen::Vector3d, Eigen::Vector3d> getLengthsToSurfacePlanes();
};



class SurfaceTally : public Tally {
public:
    void processMeasurements(TempTallyData& temp_tally_data) override;
protected:
    Eigen::Vector3d norm_;

    bool willPassThrough();

    double calculateEntranceCosine();
    virtual double calculateIntersectionParameter(double entrance_cosine) = 0;
    Eigen::Vector3d calculateIntersectionPoint(double t);
    static bool isPhotonParallelToPlane(double entrance_cosine, double EPSILON);
    bool isOutOfRange(double t);
    virtual bool isOutsideOfSurface(const Eigen::Vector3d& intersection_point) = 0;
};

class DiscSurfaceTally : public SurfaceTally {
public:
    DiscSurfaceTally(Eigen::Vector3d center, Eigen::Vector3d norm, double radius, std::unique_ptr<QuantityContainer> quantities);
private:
    Eigen::Vector3d center_;
    double radius_;

    void setArea();

    double calculateIntersectionParameter(double entrance_cosine) override;
    bool isOutsideOfSurface(const Eigen::Vector3d& intersection_point) override;
};

class RectangularSurfaceTally : public SurfaceTally {
public:
    RectangularSurfaceTally(Eigen::Vector3d corner, Eigen::Vector3d edge1, Eigen::Vector3d edge2, std::unique_ptr<QuantityContainer> quantities);
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

#endif //MCXRAYTRANSPORT_TALLY_H
