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
    PhysicsEngine(VoxelGrid& voxel_grid, const PhysicsEngineDataService& data_service) : voxel_grid_(voxel_grid), interaction_data_(data_service.getInteractionData()),
    uniform_dist_(0.0, 1.0) {}


    void transportPhoton(Photon& photon) {
        while (!photon.isTerminated()) {
            transportPhotonOneStep(photon);
        }
    }

    void transportPhotonOneStep(Photon& photon) {
        // delta tracking algorithm
        double photon_energy = photon.getEnergy();
        double max_cross_section = (*interaction_data_.total_max_interpolator)(photon_energy);
        double free_path_length = getFreePath(max_cross_section);

        // move photon to distance of free path length
        photon.move(free_path_length);

        std::array<int, 3> current_voxel_index{};
        // Check if photon is still in voxel grid. If yes, kill photon
        try {
            current_voxel_index = voxel_grid_.getVoxelIndex(photon.getPosition());
        } catch (const std::out_of_range &e) {
            photon.terminate();
            return;
        }

        // get material of new voxel
        int current_element = voxel_grid_.getVoxelElement(current_voxel_index);

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
            voxel_grid_.addVoxelDose(current_voxel_index, photon_energy);
            photon.terminate();  // not considering secondary electrons for now
        } else if (interaction_type == "coherent") {
            simulateCoherentScattering(photon, current_element);
        } else if (interaction_type == "incoherent") {
            voxel_grid_.addVoxelDose(current_voxel_index, simulateIncoherentScattering(photon, current_element));
            photon.terminate();  // not considering secondary electrons for now
        } else {
            throw std::runtime_error("Interaction type not recognized");
        }
    }

    std::string getInteractionType(double photon_energy, int element, double total_cross_section) {


        // get cross sections for current element
        double coherent_scattering_cross_section = (*interaction_data_.interaction_data_map.at(element).coherent_scattering_interpolator)(photon_energy);
        double incoherent_scattering_cross_section = (*interaction_data_.interaction_data_map.at(element).incoherent_scattering_interpolator)(photon_energy);
// these values seem to small while debugging


        // sample interaction type
        double p_coherent = coherent_scattering_cross_section / total_cross_section;
        double p_incoherent = incoherent_scattering_cross_section / total_cross_section;


        // This is a special case of inversion sampling that is done in DiscreteDistribution.
        // In this case, the x values do not matter, only the y values
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
    VoxelGrid& voxel_grid_;
    UniformDist uniform_dist_;
    InteractionData interaction_data_;


    // get free path from using inverse sampling on the max cross section
    double getFreePath(double max_cross_section) {
        double free_path = -log(uniform_dist_.sample()) / max_cross_section;
        return free_path;
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


        // rotate photon direction
        photon.rotate(theta, phi);
    }

    double simulateIncoherentScattering(Photon& photon, int element) {
        double E = photon.getEnergy();
        double kappa = E / (ELECTRON_MASS * SPEED_OF_LIGHT * SPEED_OF_LIGHT);
        double a_1 = log(1 + 2 * kappa);
        double a_2 = (2 * kappa) * (1 + kappa) / pow((1 + 2 * kappa), 2);

        double pi_1 = a_1 / (a_1 + a_2);
        // pi_2 = 1 - pi_1

        double tau_min = 1 / (1 + 2 * kappa);
        double tau_max = 1;
        double tau;

        // sample phi
        double phi = 2*PI*uniform_dist_.sample();


        while(true) {
            // sample cos(theta) (PENELOPE-2018 p. 71)

            // special case of inversion sampling where x values do not matter,
            // only y values (binary search isn't necessary,
            // implement more efficient DiscreteDist.sample())
            double random_number_1 = uniform_dist_.sample();
            double random_number_2 = uniform_dist_.sample();

            if (random_number_1 < pi_1) {
                // i = 1
                tau = pow(tau_min, random_number_2);
            } else {
                // i = 2
                tau = sqrt((tau * tau) + random_number_2 * (1 - (tau * tau)));
            }

            double cos_theta = 1 - (1 - tau) / (kappa * tau);
            double E_prime = E / (1 + kappa * (1 - cos_theta));

            // calculate q_c for scattering function
            double q_c = 1 / SPEED_OF_LIGHT * sqrt((E_prime * E_prime) + (E * E) - 2 * E_prime * E * cos_theta);

            // compute scattering function at q_c
            double scattering_function = (*interaction_data_.interaction_data_map.at(
                    element).incoherent_scattering_interpolator)(q_c);

            double random_number_3 = uniform_dist_.sample();

            if (random_number_3 < scattering_function) {
                // accept
                photon.setEnergy(E_prime);
                photon.rotate(acos(cos_theta), phi);
                return E-E_prime;
            }
        }



    }






};

#endif