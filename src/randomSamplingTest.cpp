#include "source.h"
#include "physics_engine_data_service.h"
#include <probability_dist.h>
#include <helper.h>
#include <matplot/matplot.h>
#include <cmath>
#include "interpolators.h"

namespace plt = matplot;

int main() {
    // Load in spectrum data from csv
    Eigen::MatrixXd spectrum_data = load_csv<Eigen::MatrixXd>("data/filtered_spectrum.csv");

    // sample energy from spectrum using inversion method
    DiscreteDist energy_spectrum(spectrum_data, "inversion");
    std::vector<double> sampled_energies;
    for (int i = 0; i < 100000; ++i) {
        sampled_energies.push_back(energy_spectrum.sample());
    }
    double mean = std::accumulate(sampled_energies.begin(), sampled_energies.end(), 0.0) / sampled_energies.size();
    std::cout << "Sampled Mean: " << mean << std::endl;

    // Calculate the expectation value from the spectrum data
    std::cout << "Expectation Value: " << energy_spectrum.getExpectationValue() << std::endl;







    Eigen::VectorXd energies = spectrum_data.col(0);
    Eigen::VectorXd cross_sections = spectrum_data.col(1);



    // plot the spectrum data
    plt::plot(energies, cross_sections);
    plt::show();

};