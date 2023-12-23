#include "Core/physics_engine.h"


bool PhysicsEngineHelpers::areCollinearAndSameDirection(const Eigen::Vector3d& vec1, const Eigen::Vector3d& vec2) {
    // The vectors are collinear and pointing in the same direction
    // if their normalized forms are approximately equal
    double mag_diff = (vec1.normalized() - vec2.normalized()).norm();
    return mag_diff < EPSILON;
}

PhysicsEngine::PhysicsEngine(ComputationalDomain& comp_domain, InteractionData& interaction_data):
                             comp_domain_(comp_domain), interaction_data_(interaction_data),
                             uniform_dist_(0.0, 1.0), photoelectric_effect_(std::make_shared<PhotoelectricEffect>()),
                             coherent_scattering_(std::make_shared<CoherentScattering>()),
                             incoherent_scattering_(std::make_shared<IncoherentScattering>()) {}

void PhysicsEngine::transportPhoton(Photon& photon) {
    std::vector<TempSurfaceTallyData> temp_surface_tally_data_per_photon;
    std::vector<TempVolumeTallyData> temp_volume_tally_data_per_photon;
    std::vector<TempVoxelData> temp_voxel_data_per_photon;

    while (!photon.isTerminated()) {
        transportPhotonOneStep(photon, temp_surface_tally_data_per_photon, temp_volume_tally_data_per_photon, temp_voxel_data_per_photon);
    }
    processTallies(temp_surface_tally_data_per_photon, temp_volume_tally_data_per_photon, temp_voxel_data_per_photon);
}

void PhysicsEngine::transportPhotonOneStep(Photon& photon, std::vector<TempSurfaceTallyData>& temp_surface_tally_data_per_photon,
                                           std::vector<TempVolumeTallyData>& temp_volume_tally_data_per_photon,
                                           std::vector<TempVoxelData>& temp_voxel_data_per_photon) {

    TempSurfaceTallyData temp_surface_tally_data;
    TempVolumeTallyData temp_volume_tally_data;

    // delta tracking algorithm
    temp_surface_tally_data.initial_photon = temp_volume_tally_data.initial_photon = photon;
    double photon_energy = photon.getEnergy();
    double max_cross_section = interaction_data_.interpolateMaxTotalCrossSection(photon_energy);

    // move photon to distance of free path length
    Eigen::Vector3d initial_position = photon.getPosition();
    double free_path_length = getFreePath(max_cross_section);
    temp_surface_tally_data.free_path = temp_volume_tally_data.free_path = free_path_length;
    photon.move(free_path_length);

    // Check if photon is still in comp domain. If no, kill photon
    if (!comp_domain_.isInComputationalDomain(photon.getPosition())) {
        updateTempTallyPerPhoton(temp_surface_tally_data_per_photon, temp_volume_tally_data_per_photon,
                                 temp_surface_tally_data, temp_volume_tally_data);
        processPhotonOutsideVoxelGrid(photon);
        return;
    }

    // get material of new voxel
    Voxel& current_voxel = comp_domain_.getVoxel(photon.getPosition());
    int current_material_id = current_voxel.materialID;

    // get total cross section for current material
    Material& current_material = interaction_data_.getMaterialFromId(current_material_id);
    double total_cross_section = (current_material.getData().interpolateTotalCrossSection(photon_energy));

    // sample delta scattering
    bool delta_scattering = isDeltaScatter(total_cross_section, max_cross_section);
    if (!delta_scattering) {
        temp_surface_tally_data.isInteract = temp_volume_tally_data.isInteract = true;
        setInteractionType(photon, current_material, total_cross_section);
        photon.setPrimary(false); // photon has interacted
        double energy_deposited = photon.interact(current_material);

        TempVoxelData temp_voxel_data(current_voxel);
        temp_voxel_data.energy_deposited = energy_deposited;
        temp_volume_tally_data.energy_deposited = energy_deposited;
        temp_volume_tally_data.final_photon = photon;

        temp_voxel_data_per_photon.push_back(temp_voxel_data); // not in updateTempTallyPerPhoton because of out of bounds case
    }
    updateTempTallyPerPhoton(temp_surface_tally_data_per_photon, temp_volume_tally_data_per_photon,
                             temp_surface_tally_data, temp_volume_tally_data);
}


void PhysicsEngine::setInteractionType(Photon& photon, Material& material, double total_cross_section) {

    double photon_energy = photon.getEnergy();

    // get cross sections for current element
    double coherent_scattering_cross_section = material.getData().interpolateCoherentScatteringCrossSection(photon_energy);
    double incoherent_scattering_cross_section = material.getData().interpolateIncoherentScatteringCrossSection(photon_energy);

    // sample interaction type
    double p_coherent = coherent_scattering_cross_section / total_cross_section;
    double p_incoherent = incoherent_scattering_cross_section / total_cross_section;

    // This is a special case of inversion sampling that is done in DiscreteDistribution.
    // In this case, the x values do not matter, only the y values
    double random_number = uniform_dist_.sample();

    if (random_number < p_coherent) {
        photon.addCoherentScatter();
        photon.setInteractionBehavior(coherent_scattering_);
    }
    else if (random_number < p_coherent + p_incoherent) {
        photon.addIncoherentScatter();
        photon.setInteractionBehavior(incoherent_scattering_);
    }
    else {
        photon.setInteractionBehavior(photoelectric_effect_);
    }
}

void PhysicsEngine::addVolumeTallies(std::vector<std::unique_ptr<VolumeTally>>&& volume_tallies) {
    thread_local_volume_tallies_.push_back(std::move(volume_tallies));
}

void PhysicsEngine::addSurfaceTallies(std::vector<std::unique_ptr<SurfaceTally>>&& surface_tallies) {
    thread_local_surface_tallies_.push_back(std::move(surface_tallies));
}

void PhysicsEngine::initializeVoxelData() {
    std::vector<TempVoxelData> temp_voxel_data;
    thread_local_voxel_data_.push_back(temp_voxel_data);
}

std::vector<VolumeQuantityContainer> PhysicsEngine::getVolumeQuantityContainers() {
    // return sum of all thread local tallies
    int num_of_volume_tallies = thread_local_volume_tallies_[0].size();
    int num_of_threads = thread_local_volume_tallies_.size();
    std::vector<VolumeQuantityContainer> combined_volume_containers(num_of_volume_tallies);

    for (int i = 0; i < num_of_threads; ++i) {
        for (int j = 0; j < num_of_volume_tallies; ++j) {
            auto thread_local_volume_container = thread_local_volume_tallies_[i][j]->getVolumeQuantityContainer();
            combined_volume_containers[j] = combined_volume_containers[j] + thread_local_volume_container;
        }
    }

    return combined_volume_containers;
}

std::vector<SurfaceQuantityContainer> PhysicsEngine::getSurfaceQuantityContainers() {
    // return sum of all thread local tallies
    int num_of_surface_tallies = thread_local_surface_tallies_[0].size();
    int num_of_threads = thread_local_surface_tallies_.size();
    std::vector<SurfaceQuantityContainer> combined_surface_containers(num_of_surface_tallies);

    for (int i = 0; i < num_of_threads; ++i) {
        for (int j = 0; j < num_of_surface_tallies; ++j) {
            auto thread_local_surface_container = thread_local_surface_tallies_[i][j]->getSurfaceQuantityContainer();
            combined_surface_containers[j] = combined_surface_containers[j] + thread_local_surface_container;
        }
    }

    return combined_surface_containers;
}

void PhysicsEngine::addVoxelDataToComputationalDomain() {
    for (auto &thread_local_voxel_data: thread_local_voxel_data_) {
        for (auto &temp_voxel_data: thread_local_voxel_data) {
            temp_voxel_data.voxel.dose.addValue(temp_voxel_data.energy_deposited);
        }
    }
}

double PhysicsEngine::getFreePath(double max_cross_section) {
    double free_path = -log(uniform_dist_.sample()) /
                       (max_cross_section);
    return free_path;
}

void PhysicsEngine::processPhotonOutsideVoxelGrid(Photon& photon) {
    photon.terminate();
}

bool PhysicsEngine::isDeltaScatter(double cross_section, double max_cross_section) {
    double p_delta_scatter = 1 - cross_section / max_cross_section;
    return uniform_dist_.sample() < p_delta_scatter;
}

void PhysicsEngine::updateTempTallyPerPhoton(std::vector<TempSurfaceTallyData>& temp_surface_tally_data_per_photon,
                              std::vector<TempVolumeTallyData>& temp_volume_tally_data_per_photon,
                              TempSurfaceTallyData& temp_surface_tally_data, TempVolumeTallyData& temp_volume_tally_data) {
    temp_surface_tally_data_per_photon.push_back(temp_surface_tally_data);
    temp_volume_tally_data_per_photon.push_back(temp_volume_tally_data);
}

void PhysicsEngine::processTallies(std::vector<TempSurfaceTallyData>& temp_surface_tally_data_per_photon,
                                   std::vector<TempVolumeTallyData>& temp_volume_tally_data_per_photon,
                                    std::vector<TempVoxelData>& temp_voxel_data_per_photon) {
    for (auto &surface_tally: thread_local_surface_tallies_[omp_get_thread_num()]) {
        for (auto &temp_surface_tally_data: temp_surface_tally_data_per_photon) {
            surface_tally->processMeasurements(temp_surface_tally_data);
        }
    }
    for (auto &volume_tally: thread_local_volume_tallies_[omp_get_thread_num()]) {
        for (auto &temp_volume_tally_data: temp_volume_tally_data_per_photon) {
            volume_tally->processMeasurements(temp_volume_tally_data);
        }
    }
    for (auto &temp_voxel_data: temp_voxel_data_per_photon) {
        if (temp_voxel_data.energy_deposited != 0.0) {
            thread_local_voxel_data_[omp_get_thread_num()].push_back(temp_voxel_data);
        }
    }
}
