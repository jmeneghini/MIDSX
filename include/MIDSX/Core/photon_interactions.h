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
private:
    static ProbabilityDist::Discrete createFormFactorDistribution(Eigen::MatrixXd form_factor_matrix, double x_min, double x_max);
    double sampleThetaFromCoherentScatteringDCS(const ProbabilityDist::Discrete& form_factor_dist, double x_max);
};

class IncoherentScattering : public ParticleInteractionBehavior {
public:
    double interact(Particle& photon, Material& material) override;
private:
    double changeTrajectoryAndReturnEnergyForCoherentScattering(Particle& photon, double mu, double k);
    double sampleMuFromH(double b, double c_0);
    static double getKNAcceptanceProbability(double a, double b, double mu, double k);
    static double getSFAcceptanceProbability(double x, double k, Material& material);
    static double getResultingEnergy(double mu, double k);
};

#endif //MCXRAYTRANSPORT_PHOTON_INTERACTIONS_H
