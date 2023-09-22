#include <MIDSX/Core.h>


int main() {
    auto data_service = std::make_shared<DataAccessObject>("data/data_sources/EPDL/EPDL.db");
    auto mat = std::make_unique<Material>("Tissue, Soft (ICRU-46)", data_service);
    auto mat_copy = *mat;
    auto mat_vec = std::vector<std::unique_ptr<Material>>();
    mat_vec.emplace_back(std::move(mat));
    auto interaction_data = InteractionData(std::move(mat_vec), data_service);

    const int N_PHOTONS = 100000000;
    const double ENERGY = 120E3;
    Eigen::Vector3d position(0, 0, 0);
    Eigen::Vector3d direction(0, 0, 1);
    IncoherentScattering scattering;


//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "openmp-use-default-none"
//#pragma omp parallel for
    for (int i = 0; i < N_PHOTONS; ++i) {
        Photon photon(position, direction, ENERGY);
        scattering.interact(photon, interaction_data, mat_copy);
    }
}