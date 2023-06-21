#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "photon.h"
#include "voxel_grid.h"
#include "vec3.h"
#include "probability_dist.h"
#include <cmath>

class PhysicsEngine {
public:
    PhysicsEngine() {}
    PhysicsEngine(Photon& photon, VoxelGrid& voxel_grid) : photon(photon), voxel_grid(voxel_grid) {
        // initialize random number generator
        uniform_dist = UniformDist(0.0, 1.0);
    }

    void transportPhoton() {
        // calculate free path length for current voxel
        while (true) {
            double max_cross_section = voxel_grid.getMaxCrossSection; // TODO: implement this function
            double l_1 =  -1/total_cross_section * log(uniform_dist.sample());

            // move photon by free path length
            photon.move(l_1);

            // check if photon is in voxel grid. If not, terminate photon
            try {
                int voxel_index = voxel_grid.getVoxelIndex(photon.getPosition());
            } catch (const std::out_of_range& e) {
                photon.terminate();
                return;
            }

            // get cross section of current voxel
            double cross_section = voxel_grid.getVoxel(voxel_index).getCrossSection();

            // sample type of interaction: delta or real
            double p_delta = (max_cross_section - cross_section) / max_cross_section;
            
            if (uniform_dist.sample() < p_delta) {
                // delta interaction
                continue;
            }
            else {
                simulateRealInteraction();
            }
        }
    }


private:
    Photon photon;
    VoxelGrid voxel_grid;
    UniformDist uniform_dist;


};

#endif