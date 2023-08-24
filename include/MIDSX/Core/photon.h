#ifndef PHOTON_H
#define PHOTON_H

#include <utility>
#include "photon_interactions.h"
#include "particle.h"
#include "interaction_data.h"
#include "particle_interaction_behavior.h"
#include <memory>

class Photon : public Particle {
public:
    Photon() = default;
    Photon(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy) : Particle(position, direction, energy) {
        interaction_behavior_ = std::make_shared<PhotoelectricEffect>();
    }

    double interact(const InteractionData& interaction_data, Material& material) {
        return interaction_behavior_->interact(*this, interaction_data, material);
    }

    void setInteractionBehavior(std::shared_ptr<ParticleInteractionBehavior> interaction_behavior) {
        interaction_behavior_ = std::move(interaction_behavior);
    }
private:
    std::shared_ptr<ParticleInteractionBehavior> interaction_behavior_;
};

#endif