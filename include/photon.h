#ifndef PHOTON_H
#define PHOTON_H

#include <utility>

#include "vec3.h"
#include "helper.h"
#include "photon_interactions.h"
#include "particle.h"
#include "physics_engine_data_service.h"
#include "particle_interaction_behavior.h"
#include <memory>

class Photon : public Particle {
public:
    Photon(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy) : Particle(position, direction, energy) {
        interaction_behavior_ = std::make_unique<PhotoelectricEffect>();
    }

    double interact(const InteractionData& interaction_data, int element) {
        return interaction_behavior_->interact(*this, interaction_data, element);
    }

    void setInteractionBehavior(std::unique_ptr<ParticleInteractionBehavior> interaction_behavior) {
        interaction_behavior_ = std::move(interaction_behavior);
    }


private:
    std::unique_ptr<ParticleInteractionBehavior> interaction_behavior_;
};

#endif