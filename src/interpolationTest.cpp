#include "source.h"
#include "physics_engine_data_service.h"
#include <probability_dist.h>
#include <helper.h>
#include <matplot/matplot.h>
#include <cmath>
#include "interpolators.h"

namespace plt = matplot;

void testSplineInterpolation(int element, const std::string& data_type) {
    PhysicsEngineDataService data("data/data_sources/EPDL/EPDL.db");
    std::vector<int> elements = {element};
    auto scattering_functions = data.getCoherentScatteringFormFactors(elements, {1E-8, 1E-4});

    Eigen::VectorXd energies = scattering_functions[element].row(0)*1E-6;
    Eigen::VectorXd cross_sections = scattering_functions[element].row(1);

    SplineInterpolator s(energies.array().log10(), cross_sections.array().log10());


    Eigen::VectorXd interpolated_energies = logspace(energies.size()*4, log10(energies.minCoeff()), log10(energies.maxCoeff()));

    Eigen::VectorXd interpolated_cross_sections(interpolated_energies.size());
    for (int i = 0; i < interpolated_energies.size(); ++i) {
        interpolated_cross_sections(i) = pow(10, s(log10(interpolated_energies(i))));
    }

    auto p = plt::loglog(energies, cross_sections, "ro", interpolated_energies, interpolated_cross_sections, "g-");
    p[0]->marker_size(5);
    p[1]->line_width(2);
    plt::show();

}

int main() {
    testSplineInterpolation(85, "photoelectric");
    return 0;




}