#include "source.h"
#include "physics_engine_data_service.h"
#include <probability_dist.h>
#include <helper.h>

int main() {
    PhysicsEngineDataService data("data/data_sources/EPDL/EPDL.db");
    std::vector<int> elements = {1, 2, 3, 4, 5, 6, 7, 8};
    std::map<int, std::map<std::string, CrossSectionData>> cross_sections = data.getCrossSections(elements);
    std::cout << "Cross sections for elements 1-8:" << std::endl;
    std::cout << "Element 1:" << std::endl;
    std::cout << "Incoherent scattering cross-sections:" << std::endl;
    std::cout << "Energies: " << std::endl;
    for (auto &energy: cross_sections[1]["photoelectric"].energies) {
        std::cout << energy << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Cross-sections: " << std::endl;
    for (auto &cross_section: cross_sections[1]["photoelectric"].crossSections) {
        std::cout << cross_section << std::endl;
    }
    std::cout << std::endl;


}