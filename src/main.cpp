#include "source.h"
#include "physics_engine_data_service.h"
#include <probability_dist.h>
#include <helper.h>
#include <matplot/matplot.h>
#include <cmath>
#include "interpolators.h"

namespace plt = matplot;

int main() {
    std::vector<int> elements = {85, 24, 13};
    PhysicsEngineDataService data_service("data/data_sources/EPDL/EPDL.db", elements);
    InteractionData data = data_service.getInteractionData();

//    auto total_max_cross_sections = data.total_max_cross_sections_matrix;
//    auto total_cross_sections_85 = data.interaction_data_map[85].total_cross_sections_matrix;
//    auto total_cross_sections_24 = data.interaction_data_map[24].total_cross_sections_matrix;
//    auto total_cross_sections_13 = data.interaction_data_map[13].total_cross_sections_matrix;
//
//    Eigen::VectorXd energies_max = total_max_cross_sections.col(0);
//    Eigen::VectorXd cross_sections_max = total_max_cross_sections.col(1);
//
//    Eigen::VectorXd energies_85 = total_cross_sections_85.col(0);
//    Eigen::VectorXd cross_sections_85 = total_cross_sections_85.col(1);
//
//    Eigen::VectorXd energies_24 = total_cross_sections_24.col(0);
//    Eigen::VectorXd cross_sections_24 = total_cross_sections_24.col(1);
//
//    Eigen::VectorXd energies_13 = total_cross_sections_13.col(0);
//    Eigen::VectorXd cross_sections_13 = total_cross_sections_13.col(1);
//
//    plt::loglog(energies_max, cross_sections_max, "ko",
//                energies_85, cross_sections_85, "r-",
//                energies_24, cross_sections_24, "g-",
//                energies_13, cross_sections_13, "b-");
//    plt::show();




}