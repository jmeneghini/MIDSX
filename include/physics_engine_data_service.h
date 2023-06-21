#include "helper.h"
#include "data_access_object.h"

// Stores cross section data for a given element
struct CrossSectionData {
    std::vector<double> energies;
    std::vector<double> crossSections;
};

class PhysicsEngineDataService {
public:
    PhysicsEngineDataService(const std::string& db_name) : dao(db_name) {}

    // Fetches cross section data for given elements
    std::map<std::string, std::map<std::string, CrossSectionData>> getCrossSections(const std::vector<std::string>& elements) {
        std::map<std::string, std::map<std::string, CrossSectionData>> data;

        for (auto &element: elements) {
            // Fetches scattering data for current element
            std::string scatter_query = "SELECT log_energy, log_coherent_scatter, log_incoherent_scatter FROM scattering WHERE element = '" + element + "'";
            std::vector<std::string> res = dao.execute_query(scatter_query);
            std::vector<double> log_scatter_energies = stringToVector(res[0]);
            std::vector<double> log_rayleigh_cross_sections = stringToVector(res[1]);
            std::vector<double> log_compton_cross_sections = stringToVector(res[2]);

            // Fetches photoabsorption data for current element
            std::string photo_query = "SELECT log_energy, log_photoabsorption FROM photoabsorption WHERE element = '" + element + "'";
            res = dao.execute_query(photo_query);
            std::vector<double> log_photo_energies = stringToVector(res[0]);
            std::vector<double> log_photo_cross_sections = stringToVector(res[1]);

            // Stores data for current element
            data[element] = {
                    {"rayleigh", {log_scatter_energies, log_rayleigh_cross_sections}},
                    {"compton", {log_scatter_energies, log_compton_cross_sections}},
                    {"photoabsorption", {log_photo_energies, log_photo_cross_sections}}
            };
        }
        return data;
    }

private:
    DataAccessObject dao;
};

#ifndef PHYSICS_ENGINE_DATA_SERVICE_H
#define PHYSICS_ENGINE_DATA_SERVICE_H

#endif //PHYSICS_ENGINE_DATA_SERVICE_H
