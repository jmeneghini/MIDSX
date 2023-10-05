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

class PhotoelectricEffect : public ParticleInteractionBehavior {
public:
    double interact(Particle& photon, Material& material) override;
};

class CoherentScattering : public ParticleInteractionBehavior {
public:
    double interact(Particle& photon, Material& material) override;
};

class IncoherentScattering : public ParticleInteractionBehavior {
public:
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
