#ifndef PHOTON_H
#define PHOTON_H

#include <utility>
#include "photon_interactions.h"
#include "particle.h"
#include "interaction_data.h"
#include "particle_interaction_behavior.h"
#include <memory>

/**
 * @brief Struct which contains the scattering history of a photon.
 */
struct PhotonScatteringHistory{
    int coherent = 0;
    int incoherent = 0;
    int total = 0;
};

/**
 * @brief Class which represents a photon. Inherits from Particle.
 *
 * Adds specific photon interacts and scattering history to the Particle class.
 */
class Photon : public Particle {
public:
    Photon() : interaction_behavior_(std::make_shared<PhotoelectricEffect>()) {}
    /**
     * @brief Constructor for the Photon class.
     *
     * @param position The initial position of the photon.
     * @param direction The initial direction of the photon.
     * @param energy The initial energy of the photon.
     */
    Photon(Eigen::Vector3d& position, Eigen::Vector3d& direction, double energy) : Particle(position, direction, energy) {
        interaction_behavior_ = std::make_shared<PhotoelectricEffect>();
    }

    /**
     * @brief Performs the current interaction behavior of the photon.
     *
     * @return The energy deposited at location of interaction.
     */
    double interact(Material& material) {
        return interaction_behavior_->interact(*this, material);
    }

    /**
     * @brief Sets the interaction behavior of the photon.
     *
     * @param interaction_behavior The interaction behavior to set.
     */
    void setInteractionBehavior(std::shared_ptr<ParticleInteractionBehavior> interaction_behavior) {
        interaction_behavior_ = std::move(interaction_behavior);
    }

    /**
     * @brief Adds a coherent scatter to the scattering history.
     */
    void addCoherentScatter() {
        scattering_history_.coherent++;
        scattering_history_.total++;
    }

    /**
     * @brief Adds an incoherent scatter to the scattering history.
     */
    void addIncoherentScatter() {
        scattering_history_.incoherent++;
        scattering_history_.total++;
    }

    /**
     * @brief Returns to total number of scatters in the scattering history.
     *
     * @return The total number of scatters in the scattering history.
     */
    int getTotalScatterCount() const {
        return scattering_history_.total;
    }

    /**
     * @brief Returns the number of coherent scatters in the scattering history.
     *
     * @return The number of coherent scatters in the scattering history.
     */
    int getCoherentScatterCount() const {
        return scattering_history_.coherent;
    }

    /**
     * @brief Returns the number of incoherent scatters in the scattering history.
     *
     * @return The number of incoherent scatters in the scattering history.
     */
    int getIncoherentScatterCount() const {
        return scattering_history_.incoherent;
    }


private:
    std::shared_ptr<ParticleInteractionBehavior> interaction_behavior_;
    PhotonScatteringHistory scattering_history_;
};

#endif