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
    std::map<int, std::map<std::string, CrossSectionData>> getCrossSections(const std::vector<int>& elements) {
        std::map<int, std::map<std::string, CrossSectionData>> data;


        for (auto &element: elements) {
            // Fetches incoherent scattering cross-sections for current element
            std::string scatter_query = "SELECT Energy, CrossSection FROM IncoherentScatteringCrossSections WHERE ElementID = " + std::to_string(element);
            std::vector<double> res = convertStringVector<double>(dao.execute_query(scatter_query));
            auto res_distributed = distributeNTimes(res, 2);
            std::vector<double> incoherent_energies = res_distributed[0];
            std::vector<double> incoherent_cross_sections = res_distributed[1];


            // Fetches coherent scattering cross-sections for current element
            std::string coherent_query = "SELECT Energy, CrossSection FROM CoherentScatteringCrossSections WHERE ElementID = " + std::to_string(element);
            res = convertStringVector<double>(dao.execute_query(coherent_query));
            res_distributed = distributeNTimes(res, 2);
            std::vector<double> coherent_energies = res_distributed[0];
            std::vector<double> coherent_cross_sections = res_distributed[1];


            // Fetches photoelectric absorption cross-sections for current element
            std::string photo_query = "SELECT Energy, CrossSection FROM TotalPhotoIonizationCrossSections WHERE ElementID = " + std::to_string(element);
            res = convertStringVector<double>(dao.execute_query(photo_query));
            res_distributed = distributeNTimes(res, 2);
            std::vector<double> photo_energies = res_distributed[0];
            std::vector<double> photo_cross_sections = res_distributed[1];


            // Stores data for current element
            data[element] = {
                    {"incoherent", {incoherent_energies, incoherent_cross_sections}},
                    {"coherent", {coherent_energies, coherent_cross_sections}},
                    {"photoelectric", {photo_energies, photo_cross_sections}}
            };
        }
        return data;
    }

    // Fetches Zs for given elements
    std::map<std::string, int> getZs(const std::vector<std::string>& elements) {
        std::map<std::string, int> data;
        for (auto &element: elements) {
            std::string query = "SELECT Z FROM elements WHERE element = '" + element + "'";
            std::vector<std::string> res = dao.execute_query(query);
            data[element] = std::stoi(res[0]);
        }
        return data;
    }

private:
    DataAccessObject dao;
};

#ifndef PHYSICS_ENGINE_DATA_SERVICE_H
#define PHYSICS_ENGINE_DATA_SERVICE_H

#endif //PHYSICS_ENGINE_DATA_SERVICE_H
