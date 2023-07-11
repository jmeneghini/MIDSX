#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "photon.h"
#include "voxel_grid.h"
#include "vec3.h"
#include "probability_dist.h"
#include "physics_engine_data_service.h"
#include "interpolators.h"
#include "photon_interactions.h"
#include <cmath>

class PhysicsEngine {
public:
    PhysicsEngine(VoxelGrid& voxel_grid, const PhysicsEngineDataService& data_service);

    // transport photon until it is absorbed or leaves the voxel grid (i.e. terminated)
    void transportPhoton(Photon& photon);

    // transport photon and simulate one interaction (photoelectric, coherent, or incoherent)
    void transportPhotonOneStep(Photon& photon);

    // set interaction type based on max cross section and total cross section of photon's current voxel
    void setInteractionType(Photon& photon, int element, double total_cross_section);


private:
    VoxelGrid& voxel_grid_;
    ProbabilityDist::Uniform uniform_dist_;
    InteractionData interaction_data_;
    std::shared_ptr<PhotoelectricEffect> photoelectric_effect_;
    std::shared_ptr<IncoherentScattering> incoherent_scattering_;
    std::shared_ptr<CoherentScattering> coherent_scattering_;


    // get free path from using inverse sampling on the max cross section
    double getFreePath(double max_cross_section, int element);

    // check if photon undergoes delta scattering or if it interacts
    bool isDeltaScatter(double cross_section, double max_cross_section);
};

#endif