#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "photon.h"
#include "computational_domain.h"
#include "probability_dist.h"
#include "interaction_data.h"
#include "interpolators.h"
#include "photon_interactions.h"
#include "volume_tally.h"
#include "voxel.h"
#include "surface_tally.h"
#include <cmath>
#include <utility>

namespace PhysicsEngineHelpers {
    bool areCollinearAndSameDirection(const Eigen::Vector3d& vec1, const Eigen::Vector3d& vec2);
}


class PhysicsEngine {
public:

    /**
     * @brief Constructor for the PhysicsEngine class.
     *
     * @param comp_domain The computational domain.
     * @param interaction_data The interaction data.
     * @param volume_tallies The volume tallies.
     * @param surface_tallies The surface tallies.
     */
    PhysicsEngine(ComputationalDomain& comp_domain, InteractionData& interaction_data);

//    std::vector<std::unique_ptr<VolumeTally>>&& volume_tallies, std::vector<std::unique_ptr<SurfaceTally>>&& surface_tallies
    /**
     * @brief Transports a photon until an photoelectric interaction occurs or leaves the voxel grid (i.e. terminated).
     *
     * @param photon The photon to transport.
     */
    void transportPhoton(Photon& photon);

    /**
     * @brief Transports photon one step. Either performs a real or delta interaction.
     *
     * This is the heart of simulation. This function uses the delta tracking algorithm to transport the photon to its next interaction.
     * This requires the interaction cross sections and free path length to be sampled.
     *
     * @param photon The photon to transport.
     * @param temp_surface_tally_data_per_photon Stores the surface tally data for the photon.
     * @param temp_volume_tally_data_per_photon Stores the volume tally data for the photon.
     * @param temp_voxel_data_per_photon Stores the voxel data for the photon.
     */
    void transportPhotonOneStep(Photon& photon, std::vector<TempSurfaceTallyData>& temp_surface_tally_data_per_photon,
                                std::vector<TempVolumeTallyData>& temp_volume_tally_data_per_photon,
                                std::vector<TempVoxelData>& temp_voxel_data_per_photon);

    /**
     * @brief Sets the interaction type of the photon.
     *
     * The interaction type is either photoelectric, coherent, or incoherent.
     * This is determined by sampling the ratios of interaction cross sections to the total cross section.
     *
     * @param photon The photon to set the interaction type for.
     * @param material The material of the photon's current voxel.
     * @param total_cross_section The total cross section of the photon's current voxel.
     */
    void setInteractionType(Photon& photon, Material& material, double total_cross_section);

    /**
     * @brief Sets the volume tallies of the simulation.
     *
     * @param volume_tallies The volume tallies.
     */
    void addVolumeTallies(std::vector<std::unique_ptr<VolumeTally>>&& volume_tallies);

    /**
     * @brief Sets the surface tallies of the simulation.
     *
     * @param surface_tallies The surface tallies.
     */
    void addSurfaceTallies(std::vector<std::unique_ptr<SurfaceTally>>&& surface_tallies);

    /**
     * @brief Initializes the voxel data for a thread.
     * Exists since omp_max_num_threads() is influenced by previous parallel sections.
     * So, the number of threads that are available does not match the result of omp_get_max_threads().
     */
    void initializeVoxelData();


    /**
     * @brief Returns the volume tallies of the simulation.
     *
     * @return The volume tallies.
     */
    std::vector<VolumeQuantityContainer> getVolumeQuantityContainers();

    /**
     * @brief Returns the surface tallies of the simulation.
     *
     * @return The surface tallies.
     */
    std::vector<SurfaceQuantityContainer> getSurfaceQuantityContainers();

    /**
     * @brief Takes thread local voxel data and adds it to the computational domain.
     * Should be called after all photons have been transported. Not thread safe.
     */
    void addVoxelDataToComputationalDomain();

    /**
     * @brief Processes the domain boundary crossing of a photon by terminating the photon.
     *
     * @param photon The photon to process.
     */
    static void processPhotonOutsideVoxelGrid(Photon& photon);
private:
    ComputationalDomain& comp_domain_;
    ProbabilityDist::Uniform uniform_dist_;
    InteractionData& interaction_data_;
    std::vector<std::vector<std::unique_ptr<VolumeTally>>> thread_local_volume_tallies_;
    std::vector<std::vector<std::unique_ptr<SurfaceTally>>> thread_local_surface_tallies_;
    std::vector<std::vector<TempVoxelData>> thread_local_voxel_data_;
    std::shared_ptr<PhotoelectricEffect> photoelectric_effect_;
    std::shared_ptr<IncoherentScattering> incoherent_scattering_;
    std::shared_ptr<CoherentScattering> coherent_scattering_;

    /**
     * @brief Returns the free path length of the photon.
     *
     * The free path length is sampled using inverse sampling on the max cross section
     *
     * @param max_cross_section The max cross section of the photon's current voxel.
     * @return The free path length.
     */
    double getFreePath(double max_cross_section);

    /**
     * @brief Checks if the photon undergoes delta scattering or interacts.
     *
     * @param cross_section The total cross section of the photon's current material.
     * @param max_cross_section The max total cross section of all the materials in the computation domain.
     * @return True if the photon undergoes delta scattering, false if it interacts.
     */
    bool isDeltaScatter(double cross_section, double max_cross_section);

    /**
     * @brief Updates the temporary tallies for the photon.
     *
     * @param temp_surface_tally_data_per_photon Stores the surface tally data for the photon.
     * @param temp_volume_tally_data_per_photon Stores the volume tally data for the photon.
     * @param temp_surface_tally_data Stores the surface tally data for the photon's current voxel.
     * @param temp_volume_tally_data Stores the volume tally data for the photon's current voxel.
     */
    void updateTempTallyPerPhoton(std::vector<TempSurfaceTallyData>& temp_surface_tally_data_per_photon,
                                  std::vector<TempVolumeTallyData>& temp_volume_tally_data_per_photon,
                                    TempSurfaceTallyData& temp_surface_tally_data, TempVolumeTallyData& temp_volume_tally_data);

    /**
     * @brief Processes the tallies for the photon.
     *
     * @param temp_surface_tally_data_per_photon
     * @param temp_volume_tally_data_per_photon
     * @param temp_voxel_data_per_photon
     */
    void processTallies(std::vector<TempSurfaceTallyData>& temp_surface_tally_data_per_photon,
                                       std::vector<TempVolumeTallyData>& temp_volume_tally_data_per_photon,
                                       std::vector<TempVoxelData>& temp_voxel_data_per_photon);
};

#endif