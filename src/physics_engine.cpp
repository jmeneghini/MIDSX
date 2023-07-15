#include "physics_engine.h"

PhysicsEngine::PhysicsEngine(VoxelGrid& voxel_grid, const PhysicsEngineDataService& data_service) : voxel_grid_(voxel_grid), interaction_data_(data_service.getInteractionData()),
                                                                                                    uniform_dist_(0.0, 1.0), photoelectric_effect_(std::make_shared<PhotoelectricEffect>()),
                                                                                                            coherent_scattering_(std::make_shared<CoherentScattering>()),
                                                                                                            incoherent_scattering_(std::make_shared<IncoherentScattering>()) {};
void PhysicsEngine::transportPhoton(Photon& photon) {
    while (!photon.isTerminated()) {
        transportPhotonOneStep(photon);
    }
}

void PhysicsEngine::transportPhotonOneStep(Photon& photon) {
    // delta tracking algorithm
    double photon_energy = photon.getEnergy();
    double max_cross_section = (*interaction_data_.max_total_cs_interpolator)(photon_energy);

    // get element of current voxel
    std::array<int, 3> current_voxel_index = voxel_grid_.getVoxelIndex(photon.getPosition());
    int current_element = voxel_grid_.getVoxel(current_voxel_index).element;

    // move photon to distance of free path length
    double free_path_length = getFreePath(max_cross_section, current_element);
    photon.move(free_path_length);

    // Check if photon is still in voxel grid. If no, kill photon
    try {
        current_voxel_index = voxel_grid_.getVoxelIndex(photon.getPosition());
    } catch (const std::out_of_range &e) {
        photon.terminate();
        if (isPointingToExit(photon)) {
            voxel_grid_.addExit();
        }
        return;
    }

    // get material of new voxel
    Voxel& current_voxel = voxel_grid_.getVoxel(current_voxel_index);
    current_element = current_voxel.element;

    // get total cross section for current element
    double total_cross_section = (*interaction_data_.interaction_data_map.at(current_element).total_cs_interpolator)(
            photon_energy);

    // sample delta scattering
    bool delta_scattering = isDeltaScatter(total_cross_section, max_cross_section);
    if (delta_scattering) {
        return;
    }
    else {
        setInteractionType(photon, current_element, total_cross_section);
        double energy_deposited = photon.interact(interaction_data_, current_element);
        current_voxel.dose += energy_deposited;
    }
}


bool PhysicsEngine::isPointingToExit(Photon &photon) {
    Eigen::Vector3d exit_vector = {0, 0, 1};
    Eigen::Vector3d photon_direction = photon.getDirection();
    double dot_product = photon_direction.dot(exit_vector);
    if (dot_product > 0) {
        return true;
    }
    else {
        return false;
    }

}

void PhysicsEngine::setInteractionType(Photon& photon, int element, double total_cross_section) {

    double photon_energy = photon.getEnergy();

    // get cross sections for current element
    double coherent_scattering_cross_section = (*interaction_data_.interaction_data_map.at(element).coherent_cs_interpolator)(photon_energy);
    double incoherent_scattering_cross_section = (*interaction_data_.interaction_data_map.at(element).incoherent_cs_interpolator)(photon_energy);
// these values seem to small while debugging


    // sample interaction type
    double p_coherent = coherent_scattering_cross_section / total_cross_section;
    double p_incoherent = incoherent_scattering_cross_section / total_cross_section;

    // This is a special case of inversion sampling that is done in DiscreteDistribution.
    // In this case, the x values do not matter, only the y values
    double random_number = uniform_dist_.sample();


    if (random_number < p_coherent) {
        photon.setInteractionBehavior(coherent_scattering_);
    }
    else if (random_number < p_coherent + p_incoherent) {
        photon.setInteractionBehavior(incoherent_scattering_);
    }
    else {
        photon.setInteractionBehavior(photoelectric_effect_);
    }
}

double PhysicsEngine::getFreePath(double max_cross_section, int element) {
    double free_path = -log(uniform_dist_.sample()) /
                       (max_cross_section * interaction_data_.interaction_data_map.at(element).number_density);
    return free_path;
}

bool PhysicsEngine::isDeltaScatter(double cross_section, double max_cross_section) {
    double p_delta_scatter = 1 - cross_section / max_cross_section;
    return uniform_dist_.sample() < p_delta_scatter;
}



