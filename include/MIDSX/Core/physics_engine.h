#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "photon.h"
#include "computational_domain.h"
#include "probability_dist.h"
#include "interaction_data.h"
#include "interpolators.h"
#include "photon_interactions.h"
#include "volume_tally.h"
#include "surface_tally.h"
#include <cmath>
#include <utility>

namespace PhysicsEngineHelpers {
    bool areCollinearAndSameDirection(const Eigen::Vector3d& vec1, const Eigen::Vector3d& vec2);
}


class PhysicsEngine {
public:
    // default constructor
    PhysicsEngine(ComputationalDomain& comp_domain, InteractionData& interaction_data,
                  std::vector<std::unique_ptr<VolumeTally>>&& volume_tallies, std::vector<std::unique_ptr<SurfaceTally>>&& surface_tallies);

    // transport photon until it is absorbed or leaves the voxel grid (i.e. terminated)
    void transportPhoton(Photon& photon);

    // transport photon and simulate one interaction (photoelectric, coherent, or incoherent)
    void transportPhotonOneStep(Photon& photon, std::vector<TempSurfaceTallyData>& temp_surface_tally_data_per_photon,
                                std::vector<TempVolumeTallyData>& temp_volume_tally_data_per_photon);

    // set interaction type based on max cross section and total cross section of photon's current voxel
    void setInteractionType(Photon& photon, Material& material, double total_cross_section);

    // return tallies
    std::vector<std::unique_ptr<VolumeTally>>& getVolumeTallies();

    std::vector<std::unique_ptr<SurfaceTally>>& getSurfaceTallies();

    // process photon if it is outside the voxel grid
    static void processPhotonOutsideVoxelGrid(Photon& photon);
private:
    ComputationalDomain& comp_domain_;
    ProbabilityDist::Uniform uniform_dist_;
    InteractionData& interaction_data_;
    std::vector<std::unique_ptr<VolumeTally>> volume_tallies_;
    std::vector<std::unique_ptr<SurfaceTally>> surface_tallies_;
    std::shared_ptr<PhotoelectricEffect> photoelectric_effect_;
    std::shared_ptr<IncoherentScattering> incoherent_scattering_;
    std::shared_ptr<CoherentScattering> coherent_scattering_;


    // get free path from using inverse sampling on the max cross section
    double getFreePath(double max_cross_section);

    // check if photon undergoes delta scattering or if it interacts
    bool isDeltaScatter(double cross_section, double max_cross_section);

    void updateTempTallyPerPhoton(std::vector<TempSurfaceTallyData>& temp_surface_tally_data_per_photon,
                                  std::vector<TempVolumeTallyData>& temp_volume_tally_data_per_photon,
                                    TempSurfaceTallyData& temp_surface_tally_data, TempVolumeTallyData& temp_volume_tally_data);

    void processTallies(std::vector<TempSurfaceTallyData> temp_surface_tally_data_per_photon,
                                       std::vector<TempVolumeTallyData> temp_volume_tally_data_per_photon);
};

#endif