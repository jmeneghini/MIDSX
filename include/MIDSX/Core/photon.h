#ifndef PHOTON_H
#define PHOTON_H

#include <utility>
#include "photon_interactions.h"
#include "particle.h"
#include "interaction_data.h"
#include "particle_interaction_behavior.h"
#include <memory>

struct PhotonScatteringHistory{
    int coherent = 0;
    int incoherent = 0;
    int total = 0;
};

class Photon : public Particle {
public:
    Photon() : interaction_behavior_(std::make_shared<PhotoelectricEffect>()) {}
    Photon(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy) : Particle(position, direction, energy) {
        interaction_behavior_ = std::make_shared<PhotoelectricEffect>();
    }

    double interact(const InteractionData& interaction_data, Material& material) {
        return interaction_behavior_->interact(*this, interaction_data, material);
    }

    void setInteractionBehavior(std::shared_ptr<ParticleInteractionBehavior> interaction_behavior) {
        interaction_behavior_ = std::move(interaction_behavior);
    }

    void addCoherentScatter() {
        scattering_history_.coherent++;
        scattering_history_.total++;
    }

    void addIncoherentScatter() {
        scattering_history_.incoherent++;
        scattering_history_.total++;
    }

    int getTotalScatterCount() const {
        return scattering_history_.total;
    }

    int getCoherentScatterCount() const {
        return scattering_history_.coherent;
    }

    int getIncoherentScatterCount() const {
        return scattering_history_.incoherent;
    }


private:
    std::shared_ptr<ParticleInteractionBehavior> interaction_behavior_;
    PhotonScatteringHistory scattering_history_;
};

#endif