#include "source.h"
#include "physics_engine_data_service.h"
#include <probability_dist.h>
#include <helper.h>
#include <matplot/matplot.h>
#include <cmath>
#include "interpolators.h"

namespace plt = matplot;

int main() {
    std::vector<int> elements = {13};
    PhysicsEngineDataService data_service("data/data_sources/EPDL/EPDL.db", elements);
    InteractionData data = data_service.getInteractionData();

    auto total_cross_sections_13 = data.interaction_data_map[13].total_cross_sections_matrix;
    auto incoherent_cross_sections_13 = data.interaction_data_map[13].incoherent_cross_sections_matrix;
    auto coherent_cross_sections_13 = data.interaction_data_map[13].coherent_cross_sections_matrix;
    auto photoelectric_cross_sections_13 = data.interaction_data_map[13].photo_cross_sections_matrix;

    Eigen::VectorXd energies_13 = total_cross_sections_13.col(0);
    Eigen::VectorXd cross_sections_13 = total_cross_sections_13.col(1);

    Eigen::VectorXd energies_13_incoherent = incoherent_cross_sections_13.col(0);
    Eigen::VectorXd cross_sections_13_incoherent = incoherent_cross_sections_13.col(1);

    Eigen::VectorXd energies_13_coherent = coherent_cross_sections_13.col(0);
    Eigen::VectorXd cross_sections_13_coherent = coherent_cross_sections_13.col(1);

    Eigen::VectorXd energies_13_photoelectric = photoelectric_cross_sections_13.col(0);
    Eigen::VectorXd cross_sections_13_photoelectric = photoelectric_cross_sections_13.col(1);

    plt::loglog(energies_13, cross_sections_13, "ko",
                energies_13_incoherent, cross_sections_13_incoherent, "r-",
                energies_13_coherent, cross_sections_13_coherent, "g-",
                energies_13_photoelectric, cross_sections_13_photoelectric, "b-");

    // add legend
    plt::legend({"Total", "Incoherent", "Coherent", "Photoelectric"});


    plt::xlabel("Energy [eV]");
    plt::ylabel("Cross section [barn]");
    plt::show();




}