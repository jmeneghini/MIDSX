#include "Core/tally.h"

std::unique_ptr<QuantityContainer> Tally::getQuantityContainer() {
    return std::move(quantities_);
}

void Tally::processMeasurements(TempTallyData& temp_tally_data) {
    temp_tally_data_ = temp_tally_data;
    if (willPassThrough()) {
        quantities_->measureAll(temp_tally_data_);
    }
}

bool SurfaceTally::willPassThrough() {
    double entrance_cosine = calculateEntranceCosine();
    double EPSILON = 1e-9;
    if (isPhotonParallelToPlane(entrance_cosine, EPSILON) || entrance_cosine < 0) {
        return false;
    }

    double t = calculateIntersectionParameter(entrance_cosine);
    if (isOutOfRange(t)) {
        return false;
    }

    Eigen::Vector3d intersection_point = calculateIntersectionPoint(t);
    if (isOutsideOfSurface(intersection_point)) {
        return false;
    }

    temp_tally_data_.entrance_cosine = entrance_cosine;
    return true;
}

double SurfaceTally::calculateEntranceCosine() {
    return temp_tally_data_.initial_photon.getDirection().dot(norm_);
}

Eigen::Vector3d SurfaceTally::calculateIntersectionPoint(double t) {
    return temp_tally_data_.initial_photon.getPosition() + t * temp_tally_data_.initial_photon.getDirection();
}

bool SurfaceTally::isPhotonParallelToPlane(double entrance_cosine, double EPSILON) {
    return std::abs(entrance_cosine) < EPSILON;
}

bool SurfaceTally::isOutOfRange(double t) {
    return t > temp_tally_data_.free_path || t < 0;
}

DiscSurfaceTally::DiscSurfaceTally(Eigen::Vector3d center, Eigen::Vector3d norm, const double radius, std::unique_ptr<QuantityContainer> quantities):
center_(std::move(center)), radius_(radius) {
    quantities_ = std::move(quantities);
    norm_ = std::move(norm);
    setArea();
}

void DiscSurfaceTally::setArea() {
    quantities_->setArea(M_PI * radius_ * radius_);
}

double DiscSurfaceTally::calculateIntersectionParameter(double entrance_cosine) {
    return (center_ - temp_tally_data_.initial_photon.getPosition()).dot(norm_) / entrance_cosine;
}

bool DiscSurfaceTally::isOutsideOfSurface(const Eigen::Vector3d& intersection_point) {
    return (intersection_point - center_).norm() > radius_;
}

RectangularSurfaceTally::RectangularSurfaceTally(Eigen::Vector3d corner, Eigen::Vector3d edge1, Eigen::Vector3d edge2, std::unique_ptr<QuantityContainer> quantities):
corner_(std::move(corner)), edge1_(std::move(edge1)), edge2_(std::move(edge2)) {
    handleOrthogonalEdges();
    quantities_ = std::move(quantities);
    setArea();
    setBasisDotProducts();
}

void RectangularSurfaceTally::handleOrthogonalEdges() {
    if (!areEdgesOrthogonal()) {
        throw std::invalid_argument("Edges are not orthogonal");
    }
}

void RectangularSurfaceTally::setArea() {
    norm_ = (edge1_.cross(edge2_)).normalized();
    quantities_->setArea(edge1_.cross(edge2_).norm());
}

void RectangularSurfaceTally::setBasisDotProducts() {
    edge1_dot_edge1_ = edge1_.dot(edge1_);
    edge2_dot_edge2_ = edge2_.dot(edge2_);
}

bool RectangularSurfaceTally::areEdgesOrthogonal() {
    double EPSILON = 1e-9;
    return std::abs(edge1_.dot(edge2_)) < EPSILON;
}

double RectangularSurfaceTally::calculateIntersectionParameter(double entrance_cosine) {
    return (corner_ - temp_tally_data_.initial_photon.getPosition()).dot(norm_) / entrance_cosine;
}

bool RectangularSurfaceTally::isOutsideOfSurface(const Eigen::Vector3d& intersection_point) {
    // Simple algorithm for checking if intersection point is outside of rectangle.
    // Represents intersection in terms of rectangle's basis vectors.
    Eigen::Vector3d corner_to_intersection_point = intersection_point - corner_;
    double edge1_dot_corner_to_intersection_point = edge1_.dot(corner_to_intersection_point);
    double edge2_dot_corner_to_intersection_point = edge2_.dot(corner_to_intersection_point);
    return edge1_dot_corner_to_intersection_point < 0 || edge1_dot_corner_to_intersection_point > edge1_dot_edge1_ ||
           edge2_dot_corner_to_intersection_point < 0 || edge2_dot_corner_to_intersection_point > edge2_dot_edge2_;
}