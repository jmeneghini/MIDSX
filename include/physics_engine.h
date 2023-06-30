#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "photon.h"
#include "voxel_grid.h"
#include "vec3.h"
#include "probability_dist.h"
#include "physics_engine_data_service.h"
#include "spline.h"
#include <cmath>

class PhysicsEngine {
public:
    PhysicsEngine(const VoxelGrid& voxel_grid, const PhysicsEngineDataService& data_service, const XraySpectrum& xray_spectrum) : voxel_grid(voxel_grid), data_service(data_service),
                                                                                               uniform_dist(0.0, 1.0), xray_spectrum(xray_spectrum) {
        initialize_data_maps();
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

    std::string getInteractionType() {
        float photon_energy = photon.getEnergy(); // TODO: decide how photon should be handled (pass to transportPhoton or make it a member variable)
        int current_element = voxel_grid.getVoxel(current_voxel_index).getElement();

        // get cross sections for current element
        double coherent_scattering_cross_section = coherent_scattering_cross_sections_spline_map.at(current_element)(photon_energy);
        double incoherent_scattering_cross_section = incoherent_scattering_cross_sections_spline_map.at(current_element)(photon_energy);
        double photoelectric_cross_section = linearInterpolation(cross_sections_map.at(current_element).at("photoelectric"), photon_energy);

        // calculate total cross section
        double total_cross_section = coherent_scattering_cross_section + incoherent_scattering_cross_section + photoelectric_cross_section;

        // sample interaction type
        double p_coherent = coherent_scattering_cross_section / total_cross_section;
        double p_incoherent = incoherent_scattering_cross_section / total_cross_section;
        double p_photoelectric = photoelectric_cross_section / total_cross_section;

        Eigen::Matrix<double, 3, 2> interaction_type_probabilities;

        interaction_type_probabilities << 1.0, p_coherent,
                                          2.0, p_coherent + p_incoherent,
                                          3.0, p_coherent + p_incoherent + p_photoelectric;

        DiscreteDist interaction_type_dist(interaction_type_probabilities);
        int interaction_type_int = static_cast<int>(interaction_type_dist.sample());
        return interactionIntToString(interaction_type_int);

        // TODO: was absolutely not sober when I wrote this. Don't trust anything you see here
    }


private:
    VoxelGrid voxel_grid;
    UniformDist uniform_dist;
    PhysicsEngineDataService data_service;
    XraySpectrum xray_spectrum;
    std::map<int, SplineFunction> coherent_scattering_cross_sections_spline_map;
    std::map<int, SplineFunction> incoherent_scattering_cross_sections_spline_map;
    std::map<int, std::map<std::string, Eigen::MatrixXd>> cross_sections_map;

    void initialize_data_maps() {
        // get all elements from voxel grid
        std::vector<int> elements_in_grid = voxel_grid.getElements();

        // get min and max energies from x-ray spectrum
        double min_energy = xray_spectrum.getMinEnergy();
        double max_energy = xray_spectrum.getMaxEnergy();

        // initialize cross data maps
        cross_sections_map = data_service.getCrossSections(elements_in_grid, {min_energy, max_energy});
        auto coherent_scattering_form_factors_map = data_service.getCoherentScatteringFormFactors(elements_in_grid, {min_energy, max_energy});
        auto incoherent_scattering_functions_map = data_service.getIncoherentScatteringFunctions(elements_in_grid, {min_energy, max_energy});

        // create spline interpolaters for each element (just for coho and incoho cross sections)
        for (auto& element : elements_in_grid) {
            Eigen::MatrixXd coherent_scattering_data = coherent_scattering_form_factors_map[element];
            Eigen::MatrixXd incoherent_scattering_data = incoherent_scattering_functions_map[element];

            // map trys to use default constructor of SplineFunction, which doesn't exist. To get around this, we use emplace
            coherent_scattering_cross_sections_spline_map.emplace(element, loglogSpineInterpolator(coherent_scattering_data));
            incoherent_scattering_cross_sections_spline_map.emplace(element, loglogSpineInterpolator(incoherent_scattering_data));
        }
        // other data will use linear interpolation
    }

    SplineFunction loglogSpineInterpolator(const Eigen::MatrixXd& data) {
        Eigen::VectorXd x = data.col(0);
        Eigen::VectorXd y = data.col(1);
        SplineFunction s(data(0, ), y.array().log10());
        return s;
    }

    double interpolateFromSpline(const SplineFunction& spline, double x) {
        return pow(10, spline(x));
    }

    std::string interactionIntToString(int interaction_type_int) {
        switch (interaction_type_int) {
            case 1:
                return "coherent";
            case 2:
                return "incoherent";
            case 3:
                return "photoelectric";
            default:
                throw std::invalid_argument("interaction_type_int must be 1, 2, or 3");
        }
    }



};

#endif