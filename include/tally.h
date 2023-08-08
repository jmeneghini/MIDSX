#ifndef MCXRAYTRANSPORT_TALLY_H
#define MCXRAYTRANSPORT_TALLY_H

#include "quantity.h"
#include "quantity_container.h"
#include "photon.h"
#include <Eigen/Dense>

class Tally {
public:
    virtual void processMeasurements(TempTallyData& temp_tally_data) = 0;
    std::unique_ptr<QuantityContainer> getQuantityContainer();
protected:
    TempTallyData temp_tally_data_;
    std::unique_ptr<QuantityContainer> quantities_;
};

class DiscSurfaceTally : public Tally {
public:
    DiscSurfaceTally(const Eigen::Vector3d &center, const Eigen::Vector3d &norm, const double radius, std::unique_ptr<QuantityContainer> quantities);
    void processMeasurements(TempTallyData& temp_tally_data) override;
private:
    Eigen::Vector3d center_;
    Eigen::Vector3d norm_;
    double radius_;

    bool willPassThrough();
    void setArea();

    double calculateEntranceCosine();
    double calculateIntersectionParameter(double entrance_cosine);
    Eigen::Vector3d calculateIntersectionPoint(double t);
    static bool isPhotonParallelToDiscPlane(double entrance_cosine, double EPSILON);
    bool isOutOfRange(double t);
    bool isOutsideOfDisc(const Eigen::Vector3d& intersection_point);

};

#endif //MCXRAYTRANSPORT_TALLY_H
