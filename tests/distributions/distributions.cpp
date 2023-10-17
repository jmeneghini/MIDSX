#include <MIDSX/Core.h>
#include <fstream>
#include <chrono>

// constants
double x_min_RITA = 0;
double x_max_RITA = 5;

// data
double dummy_pdf(double x, double E) {
    return 7*x*exp(-4*x) + 0.6 * exp(-12.5 * (x - 3.5)*(x - 3.5));
}

Eigen::VectorXd energy_values = Eigen::ArrayXd::LinSpaced(100, 0, 100).matrix();


int main() {
    auto data_service = DataAccessObject("data/data_sources/EPDL/EPDL.db");
    auto interaction_data = InteractionData({"Al"}, data_service);

    const int N_PHOTONS = 10000000;
    const double ENERGY = 60E3;
    Eigen::Vector3d position(0, 0, 0);
    Eigen::Vector3d direction(0, 0, 1);
    CoherentScattering scattering;

    auto material = interaction_data.getMaterial(61);

    const double ALPHA = ELECTRON_REST_MASS/(sqrt(2)*PLANCK_CONSTANT*SPEED_OF_LIGHT*1E8);
    double k = ENERGY/ELECTRON_REST_MASS;

    // get array of cos values from -1 to 1 with 0.01 spacing
    Eigen::VectorXd cos_values = Eigen::ArrayXd::LinSpaced(500, -1, 1).matrix();
    Eigen::VectorXd x_values = ALPHA*k*sqrt(1 - cos_values.array());


//     save cos and FF values to .csv file
//    std::ofstream file;
//    file.open("/home/john/Documents/MIDSXData/experiments/interaction_validation/data_files/coherent_FF_al_60keV.csv", std::ios_base::app);
//    for (int i = 0; i < x_values.size(); ++i) {
//        double FF = material.getData().interpolateCoherentFormFactor(x_values(i));
//        file << cos_values(i) << "," << FF << "\n";
//    }



#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
    auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
    {
        std::vector<double> sampled_cos_values;
        std::ofstream file;
        file.open("/home/john/Documents/MIDSXData/experiments/interaction_validation/data_files/coherent_scattering_dist_al_60keV_RITA_1E-2_percent_test.csv", std::ios_base::app);

#pragma omp for
        for (int i = 0; i < N_PHOTONS; ++i) {
            Photon photon(position, direction, ENERGY);
            scattering.interact(photon, material);
            double cos_theta = photon.getDirection().dot(direction);

            sampled_cos_values.push_back(cos_theta);
        }
#pragma omp critical
        {
            for(auto &val : sampled_cos_values)
                file << val << std::endl;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << "Duration: " << duration << " ns" << std::endl;
    std::cout << "Duration per photon: " << duration/N_PHOTONS << " ns" << std::endl;
}