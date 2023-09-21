#include "Core/volume_tally.h"

#include <utility>

void VolumeTally::processMeasurements(TempVolumeTallyData &temp_volume_tally_data) {
    temp_volume_tally_data_ = temp_volume_tally_data;
    VolumeTraversal traversal = determineVolumeTraversal();
    switch (traversal) {
        case VolumeTraversal::MISSES:
            break;
        case VolumeTraversal::LANDS_INSIDE:
            quantities_.measureAll(temp_volume_tally_data_);
            break;
        case VolumeTraversal::PASSES_THROUGH:
            temp_volume_tally_data_.energy_deposited = 0;
            quantities_.measureAll(temp_volume_tally_data_);
            break;
        case VolumeTraversal::STARTS_INSIDE_EXITS:
            temp_volume_tally_data_.energy_deposited = 0;
            temp_volume_tally_data_.already_counted = true;
            quantities_.measureAll(temp_volume_tally_data_);
            break;
        case VolumeTraversal::STARTS_INSIDE_STAYS:
            temp_volume_tally_data_.already_counted = true;
            quantities_.measureAll(temp_volume_tally_data_);
    }
}

VolumeQuantityContainer& VolumeTally::getVolumeQuantityContainer() {
    return quantities_;
}

AACuboidVolumeTally::AACuboidVolumeTally(Eigen::Vector3d  min_corner, Eigen::Vector3d  max_corner, const VolumeQuantityContainer& quantities):
        min_corner_(std::move(min_corner)), max_corner_(std::move(max_corner)) {
    quantities_ = quantities;
}

VolumeTraversal AACuboidVolumeTally::determineVolumeTraversal() {
    std::pair<double, double> entering_and_exiting_lengths = getEnteringAndExitingLengths();
    double entering_length = entering_and_exiting_lengths.first;
    double exiting_length = entering_and_exiting_lengths.second;
    double photon_free_path = temp_volume_tally_data_.free_path;

    // exits before entering (i.e. misses), behind the photon, or too far away
    if (entering_length > exiting_length || exiting_length < 0 || entering_length > photon_free_path) {
        return VolumeTraversal::MISSES;
    }
        // starts inside
    else if (entering_length <= 0) {
        if (exiting_length >= photon_free_path) {
            return VolumeTraversal::STARTS_INSIDE_STAYS;
        } else {
            return VolumeTraversal::STARTS_INSIDE_EXITS;
        }
    } else if (exiting_length < photon_free_path) {
        return VolumeTraversal::PASSES_THROUGH;
    } else {
        return VolumeTraversal::LANDS_INSIDE;
    }
}

std::pair<double, double> AACuboidVolumeTally::getEnteringAndExitingLengths() {
    std::pair<Eigen::Vector3d, Eigen::Vector3d> min_max = getLengthsToSurfacePlanes();
    double entering_length = min_max.first.maxCoeff();
    double exiting_length = min_max.second.minCoeff();
    return {entering_length, exiting_length};
}

std::pair<Eigen::Vector3d, Eigen::Vector3d> AACuboidVolumeTally::getLengthsToSurfacePlanes() {
    Photon& photon = temp_volume_tally_data_.initial_photon;
    Eigen::Vector3d photon_position = photon.getPosition();
    Eigen::Vector3d photon_direction = photon.getDirection();
    Eigen::Vector3d min_length_to_surface;
    Eigen::Vector3d max_length_to_surface;


    for (int i = 0; i < 3; i++) {
        if (std::abs(photon_direction(i)) < EPSILON) {
            min_length_to_surface(i) = -INF;
            max_length_to_surface(i) = INF;
            continue;
        }
        min_length_to_surface(i) = (min_corner_(i) - photon_position(i)) / photon_direction(i);
        max_length_to_surface(i) = (max_corner_(i) - photon_position(i)) / photon_direction(i);

        if (min_length_to_surface(i) > max_length_to_surface(i)) {
            std::swap(min_length_to_surface(i), max_length_to_surface(i));
        }
    }
    return {min_length_to_surface, max_length_to_surface};
}