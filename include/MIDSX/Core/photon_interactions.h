#ifndef MCXRAYTRANSPORT_PHOTON_INTERACTIONS_H
#define MCXRAYTRANSPORT_PHOTON_INTERACTIONS_H

#include "interaction_data.h"
#include "constants.h"
#include "probability_dist.h"
#include "particle_interaction_behavior.h"

// forward declarations to avoid circular dependencies
class Particle;

namespace PhotonInteractionHelpers {
    Eigen::MatrixXd mergeMatrices(std::vector<Eigen::MatrixXd>& matrices);
    Eigen::MatrixXd getBlockByRowValue(Eigen::MatrixXd& matrix, double rowStartValue, double rowEndValue, int column);
    }

/**
 * @brief Class which represents the photoelectric effect interaction.
 */
class PhotoelectricEffect : public ParticleInteractionBehavior {
public:
    /**
     * @brief Performs the photoelectric effect interaction.
     *
     * Ends the photon's trajectory and returns its energy.
     *
     * @param photon The photon to interact.
     * @param material The material to interact with.
     * @return The energy deposited at location of interaction.
     */
    double interact(Particle& photon, Material& material) override;
};

/**
 * @brief Class which represents the coherent scattering interaction.
 */
class CoherentScattering : public ParticleInteractionBehavior {
public:
    /**
     * @brief Performs the coherent scattering interaction.
     *
     * Utilizes the RITA algorithm to sample the scattering angle. Samples a runtime DCS from the material data to get the scattering angle. No energy is deposited.
     *
     * @param photon The photon to interact.
     * @param material The material to interact with.
     * @return The energy deposited at location of interaction. Always 0.
     */
    double interact(Particle& photon, Material& material) override;
};

/**
 * @brief Class which represents the incoherent scattering interaction.
 */
class IncoherentScattering : public ParticleInteractionBehavior {
public:
    /**
     * @brief Performs the incoherent scattering interaction.
     *
     * Uses Kahn's algorithm to sample the Klein-Nishina distribution.
     * Then performs an additional rejection sampling on the Scattering Function to get the scattering angle and new energy.
     *
     *
     * @param photon The photon to interact.
     * @param material The material to interact with.
     * @return The energy deposited at location of interaction.
     */
    double interact(Particle& photon, Material& material) override;
private:
    static bool isR1Accepted(double k, double random_number_1);
    static double getChiIfR1Accepted(double k, double random_number_2);
    static bool isR3AcceptedIfR1Accepted(double chi, double random_number_3);
    static double getKPrime(double k, double chi);
    static double getMu(double k, double k_prime);
    bool isAcceptedByScatteringFunction(double mu, double k, Material& material);
    double changeTrajectoryAndReturnEnergyForCoherentScattering(Particle& photon, double mu, double k, double k_prime);
    static double getChiIfR1Rejected(double k, double random_number_2);
    static bool isR3AcceptedIfR1Rejected(double k, double chi, double random_number_3);
};

#endif //MCXRAYTRANSPORT_PHOTON_INTERACTIONS_H
