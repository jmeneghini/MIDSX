#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "photon.h"
#include "voxel_grid.h"
#include "vec3.h"
#include "probability_dist.h"
#include "physics_engine_data_service.h"
#include "interpolators.h"
#include <cmath>

class PhysicsEngine {
public:
    PhysicsEngine(const VoxelGrid& voxel_grid, const PhysicsEngineDataService& data_service) : voxel_grid_(voxel_grid), interaction_data_(data_service.getInteractionData()),
                                                                                                                                  uniform_dist_(0.0, 1.0) {}


    void transportPhoton(Photon& photon) {
        // delta tracking algorithm
        double photon_energy = photon.getEnergy();
        double max_cross_section = (*interaction_data_.total_max_interpolator)(photon_energy);
        double free_path_length = getFreePath(photon_energy);

        // move photon to distance of free path length
        photon.move(free_path_length);

        std::array<int, 3> current_voxel_index{};
        // check if photon is still in voxel grid. If yes, kill photon
        try {
            current_voxel_index = voxel_grid_.getVoxelIndex(photon.getPosition());
        } catch (const std::out_of_range &e) {
            photon.terminate();
            return;
        }

        // get material of new voxel
        Voxel current_voxel = voxel_grid_.getVoxel(current_voxel_index);
        int current_element = current_voxel.element;

        // get total cross section for current element
        double total_cross_section = (*interaction_data_.interaction_data_map.at(current_element).total_interpolator)(
                photon_energy);

        // sample delta scattering
        bool delta_scattering = isDeltaScatter(total_cross_section, max_cross_section);
        std::string interaction_type;
        if (delta_scattering) {
            return;
        } else {
            // sample interaction type
            interaction_type = getInteractionType(photon_energy, current_element, total_cross_section);
        }

        // simulate interaction
        if (interaction_type == "photoelectric") {
            current_voxel.dose += photon.getEnergy();
            photon.terminate();  // not considering secondary electrons for now
            return;
        } else if (interaction_type == "coherent") {


        }
    }

    std::string getInteractionType(double photon_energy, int element, double total_cross_section) {


        // get cross sections for current element
        double coherent_scattering_cross_section = (*interaction_data_.interaction_data_map.at(element).coherent_scattering_interpolator)(photon_energy);
        double incoherent_scattering_cross_section = (*interaction_data_.interaction_data_map.at(element).incoherent_scattering_interpolator)(photon_energy);



        // sample interaction type
        double p_coherent = coherent_scattering_cross_section / total_cross_section;
        double p_incoherent = incoherent_scattering_cross_section / total_cross_section;


        // this is a special case of inversion sampling that is done in DiscreteDistribution. In this case, the x values do not matter, only the y values
        double random_number = uniform_dist_.sample();

        if (random_number < p_coherent) {
            return "coherent";
        }
        else if (random_number < p_coherent + p_incoherent) {
            return "incoherent";
        }
        else {
            return "photoelectric";
        }

    }


private:
    VoxelGrid voxel_grid_;
    UniformDist uniform_dist_;
    InteractionData interaction_data_;


    // get free path from using inverse sampling on the max cross section
    double getFreePath(double max_cross_section) {
        double free_path = -log(uniform_dist_.sample()) / max_cross_section;
    }

    bool isDeltaScatter(double cross_section, double max_cross_section) {
        double p_delta_scatter = 1 - cross_section / max_cross_section;
        return uniform_dist_.sample() < p_delta_scatter;
    }

    void simulateCoherentScattering(Photon& photon, int element) {
        // get coherent scattering form factor
        Eigen::MatrixXd form_factor_matrix = interaction_data_.interaction_data_map.at(element).coherent_scattering_form_factors_matrix;
        double E_max = 2*photon.getEnergy();
        double E_min = 0;

        // restrict form factor matrix to energies between E_min and E_max
        Eigen::MatrixXd restricted_form_factor_matrix = getBlockByRowValue(form_factor_matrix, E_min, E_max, 0);
        DiscreteDist form_factor_dist(restricted_form_factor_matrix);
        double sampled_E = form_factor_dist.sample();

        // sample theta
        double theta;

        while (true) {
            double cos_theta = 1 -  0.5*pow((ELECTRON_MASS*SPEED_OF_LIGHT*sampled_E/photon.getEnergy()), 2);
            double random_number = uniform_dist_.sample();
            if (random_number < (1 + cos_theta*cos_theta)/2) {
                theta = acos(cos_theta);
                break;
            }
        }

        // sample phi (0, 2pi)
        double phi = 2*PI*uniform_dist_.sample();


        // sample



    }






};

#endif