#ifndef MCXRAYTRANSPORT_PHOTON_INTERACTIONS_H
#define MCXRAYTRANSPORT_PHOTON_INTERACTIONS_H

#include "physics_engine_data_service.h"
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
    double interact(Particle& photon, const InteractionData& interaction_data, int element) override;
};

class CoherentScattering : public ParticleInteractionBehavior {
public:
    double interact(Particle& photon, const InteractionData& interaction_data, int element) override;
};

class IncoherentScattering : public ParticleInteractionBehavior {
public:
    double interact(Particle& photon, const InteractionData& interaction_data, int element) override;

    static double getEnvelopeFunctionForIncoherentSampling(double k, double x);
};



#endif //MCXRAYTRANSPORT_PHOTON_INTERACTIONS_H
