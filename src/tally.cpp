#include "tally.h"

std::unique_ptr<QuantityContainer> Tally::getQuantityContainer() {
    return std::move(quantities_);
}

DiscSurfaceTally::DiscSurfaceTally(const Eigen::Vector3d &center, const Eigen::Vector3d &norm, const double radius, std::unique_ptr<QuantityContainer> quantities):
center_(center), norm_(norm), radius_(radius) {
    quantities_ = std::move(quantities);
    setArea();
}

void DiscSurfaceTally::processMeasurements(TempTallyData& temp_tally_data) {
    temp_tally_data_ = temp_tally_data;
    if (willPassThrough()) {
        quantities_->measureAll(temp_tally_data_);
    }
}



bool DiscSurfaceTally::willPassThrough() {
    double entrance_cosine = calculateEntranceCosine();
    double EPSILON = 1e-6;
    if (isPhotonParallelToDiscPlane(entrance_cosine, EPSILON)) {
        return false;
    }

    double t = calculateIntersectionParameter(entrance_cosine);
    if (isOutOfRange(t)) {
        return false;
    }

    Eigen::Vector3d intersection_point = calculateIntersectionPoint(t);
    if (isOutsideOfDisc(intersection_point)) {
        return false;
    }

    temp_tally_data_.entrance_cosine = entrance_cosine;
    return true;
}

void DiscSurfaceTally::setArea() {
    quantities_->setArea(M_PI * radius_ * radius_);
}

double DiscSurfaceTally::calculateEntranceCosine() {
    return temp_tally_data_.initial_photon.getDirection().dot(norm_);
}

double DiscSurfaceTally::calculateIntersectionParameter(double entrance_cosine) {
    return (center_ - temp_tally_data_.initial_photon.getPosition()).dot(norm_) / entrance_cosine;
}

Eigen::Vector3d DiscSurfaceTally::calculateIntersectionPoint(double t) {
    return temp_tally_data_.initial_photon.getPosition() + t * temp_tally_data_.initial_photon.getDirection();
}

bool DiscSurfaceTally::isPhotonParallelToDiscPlane(double entrance_cosine, double EPSILON = 1e-6) {
    return entrance_cosine < EPSILON;
}

bool DiscSurfaceTally::isOutOfRange(double t) {
    return t > temp_tally_data_.free_path || t < 0;
}

bool DiscSurfaceTally::isOutsideOfDisc(const Eigen::Vector3d& intersection_point) {
    return (intersection_point - center_).norm() > radius_;
}


