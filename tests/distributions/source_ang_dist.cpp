#include <MIDSX/Core.h>
#include <fstream>
#include <chrono>



int main() {

    const int N_PHOTONS = 100000000;
    const double ENERGY = 60E3;
    Eigen::Vector3d position(0, 0, 0);
    Eigen::Vector3d direction(0, 0, 1);

    Eigen::Vector3d source_position(39.0/2, 39.0/2, 0);
    Eigen::Vector3d directionality_point(0, 0, 180);

    MonoenergeticSpectrum mono_spectrum(ENERGY);
    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<MonoenergeticSpectrum>(mono_spectrum);

    std::unique_ptr<Directionality> directionality = std::make_unique<RectangularIsotropicDirectionality>(
            RectangularIsotropicDirectionality(directionality_point,
                                               Eigen::Vector3d(39.0, 0, 0),
                                               Eigen::Vector3d(0, 39.0, 0)));

    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(PointGeometry(source_position));

    PhotonSource source(std::move(spectrum), std::move(directionality), std::move(geometry));


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
        std::vector<Eigen::Vector3d> sampled_points;
        std::ofstream file;
        file.open("/home/john/Documents/MIDSXData/experiments/source_validation/data_files/radiography_isotropic_dist.csv", std::ios_base::app);

#pragma omp for
        for (int i = 0; i < N_PHOTONS; ++i) {
            Photon photon = source.generatePhoton();
            Eigen::Vector3d photon_direction = photon.getDirection();
            Eigen::Vector3d photon_end_position = photon.getPosition() + photon_direction * (directionality_point.norm()/(photon_direction.z()));
            sampled_points.push_back(photon_end_position);
        }
#pragma omp critical
        {
            for(auto &val : sampled_points)
                file << val.x() << "," << val.y() << "," << val.z() << std::endl;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << "Duration: " << duration << " ns" << std::endl;
    std::cout << "Duration per photon: " << duration/N_PHOTONS << " ns" << std::endl;
}