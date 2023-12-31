#ifndef MCXRAYTRANSPORT_PARTICLE_INTERACTION_BEHAVIOR_H
#define MCXRAYTRANSPORT_PARTICLE_INTERACTION_BEHAVIOR_H

#include "particle.h"
#include "probability_dist.h"

/**
 * @brief Virtual class which represents the behavior of a particle when it interacts with a material.
 */
class ParticleInteractionBehavior {
public:
    virtual ~ParticleInteractionBehavior() = default;
    virtual double interact(Particle& particle, Material& material) = 0;
protected:
    ProbabilityDist::Uniform uniform_dist_{0.0, 1.0};
};

#endif //MCXRAYTRANSPORT_PARTICLE_INTERACTION_BEHAVIOR_H
