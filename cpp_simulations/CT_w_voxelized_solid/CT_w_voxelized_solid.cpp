#include <MIDSX/Core.h>
#include <pybind11/embed.h>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <omp.h>

namespace py = pybind11;

Eigen::Vector3d dim_space = Eigen::Vector3d(120.0, 120.0, 26.0);
Eigen::Vector3d body_origin = Eigen::Vector3d(44.0, 35.0, 0.0);
Eigen::Vector3d dim_body = Eigen::Vector3d(32.0, 50.0, 26.0);

DataAccessObject setupDataService() {
    return DataAccessObject("data/data_sources/EPDL/EPDL.db");
}

std::vector<std::string> initializeMaterials() {
    std::vector<std::string> materials = {};
    materials.emplace_back("Air - C5");
    materials.emplace_back("Cushion/Foam - C5");
    materials.emplace_back("Carbon fiber - C5");
    materials.emplace_back("Soft tissue - C5");
    materials.emplace_back("Heart - C5");
    materials.emplace_back("Lung - C5");
    materials.emplace_back("Liver - C5");
    materials.emplace_back("Gallbladder - C5");
    materials.emplace_back("Spleen - C5");
    materials.emplace_back("Stomach - C5");
    materials.emplace_back("Large Intestine - C5");
    materials.emplace_back("Pancreas - C5");
    materials.emplace_back("Adrenal - C5");
    materials.emplace_back("Thyroid - C5");
    materials.emplace_back("Thymus - C5");
    materials.emplace_back("Small Intestine - C5");
    materials.emplace_back("Esophagus - C5");
    materials.emplace_back("Skin - C5");
    materials.emplace_back("Breast - C5");
    materials.emplace_back("Cortical Bone - C5");
    return materials;
}

std::vector<std::unique_ptr<SurfaceTally>> initializeSurfaceTallies() {
    std::vector<std::unique_ptr<SurfaceTally>> tallies = {};
    return tallies;
}

std::vector<std::unique_ptr<VolumeTally>> initializeVolumeTallies() {
    std::vector<std::unique_ptr<VolumeTally>> tallies = {};
    return tallies;
}

Eigen::MatrixXd processEnergySpectrum() {
    Eigen::MatrixXd energy_spectrum = SourceHelpers::readCSV("data/source_distributions/RQR9_W_AL_120kVp_E_spectrum.csv");
    energy_spectrum.col(0) = energy_spectrum.col(0).array() * 1000;  // Convert from keV to eV
    return energy_spectrum;
}

PhotonSource initializeSource() {
    auto energy_spectrum = processEnergySpectrum();

//    PolyenergeticSpectrum poly_spectrum(energy_spectrum);
//    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<PolyenergeticSpectrum>(poly_spectrum);
    MonoenergeticSpectrum mono_spectrum(56.4E3);
    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<MonoenergeticSpectrum>(mono_spectrum);
//
//    std::unique_ptr<Directionality> directionality = std::make_unique<BeamDirectionality>(BeamDirectionality(Eigen::Vector3d(
//            39.0/2, 39.0/2, 180)));
    double fan_beam_thickness = 1.0;
    std::unique_ptr<Directionality> directionality = std::make_unique<RectangularIsotropicDirectionality>(
            RectangularIsotropicDirectionality(Eigen::Vector3d(dim_space.x()/2.0, body_origin.y(), dim_space.z()/2.0 - fan_beam_thickness/2.0),
                                               Eigen::Vector3d(0, dim_body.y(), 0),
                                               Eigen::Vector3d(0, 0, fan_beam_thickness)));
    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(PointGeometry(Eigen::Vector3d(dim_space.x(), dim_space.y()/2.0, dim_space.z()/2.0)));

    PhotonSource source(std::move(spectrum), std::move(directionality), std::move(geometry));
    return source;
}



#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
void runSimulation(PhotonSource& source, PhysicsEngine& physics_engine, int N_photons) {
    int j = 0;
    auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
    for (int i = 0; i < N_photons; i++) {
        Photon photon = source.generatePhoton();
        physics_engine.transportPhoton(photon);
        if (i % (N_photons / 20) == 0) {
            std::cout << "Progress: " << j << "%" << std::flush << "\r";
            j += 5;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "\nSimulation Time: " << elapsed.count() << " s" << std::endl;
    std::cout << "Time per photon: " << elapsed.count() / N_photons << " s" << std::endl;
    std::cout << "\n" << std::endl;
}
#pragma clang diagnostic pop

void displayVolumeTallyResults(const std::vector<std::unique_ptr<VolumeTally>>& volume_tallies, int N_photons, ComputationalDomain& comp_domain) {
    int i = 1;
    for (auto &tally: volume_tallies) {
        std::cout << "Volume tally " << i << ":" << std::endl;
        std::cout << "----------------" << std::endl;
        auto quantity_container = tally->getVolumeQuantityContainer();
        auto vector_quantities = quantity_container.getVectorQuantities();
        auto count_quantities = quantity_container.getCountQuantities();
        for (auto &vector_quantity: vector_quantities) {
            std::cout << VectorVolumeQuantityHelper::toString(vector_quantity.first) << ":" << std::endl;
            std::cout << "  Total: " << vector_quantity.second.getTotalValues().getSum() / N_photons << " +- "
                      << vector_quantity.second.getTotalValues().getSumSTD() / N_photons << std::endl;
            std::cout << "  Primary: " << vector_quantity.second.getPrimaryValues().getSum() / N_photons << " +- "
                      << (vector_quantity.second.getPrimaryValues().getSum() / vector_quantity.second.getPrimaryValues().getCountSTD()) / N_photons << std::endl;
            std::cout << "  Single Coherent: "
                      << vector_quantity.second.getSingleCoherentScatterValues().getSum() / N_photons << " +- "
                      << (vector_quantity.second.getSingleCoherentScatterValues().getSum() / vector_quantity.second.getSingleCoherentScatterValues().getCountSTD()) / N_photons << std::endl;
            std::cout << "  Single Incoherent: "
                      << vector_quantity.second.getSingleIncoherentScatterValues().getSum() / N_photons << " +- "
                      << vector_quantity.second.getSingleIncoherentScatterValues().getSumSTD() / N_photons << std::endl;
            std::cout << "  Multiple scatter: "
                      << vector_quantity.second.getMultipleScatterValues().getSum() / N_photons << " +- "
                      << vector_quantity.second.getMultipleScatterValues().getSumSTD() / N_photons << std::endl << "\n";
        }

        for (auto &count_quantity: count_quantities) {
            std::cout << CountVolumeQuantityHelper::toString(count_quantity.first) << ":" << std::endl;
            std::cout << " Total: " << count_quantity.second.getTotalValues().getCount() << " +- "
                      << count_quantity.second.getTotalValues().getCountSTD() << std::endl;
            std::cout << "  Primary: " << count_quantity.second.getPrimaryValues().getCount() << " +- "
                      << count_quantity.second.getPrimaryValues().getCountSTD() << std::endl;
            std::cout << "  Single Coherent: "
                      << count_quantity.second.getSingleCoherentScatterValues().getCount() << " +- "
                      << count_quantity.second.getSingleCoherentScatterValues().getCountSTD() << std::endl;
            std::cout << "  Single Incoherent: "
                      << count_quantity.second.getSingleIncoherentScatterValues().getCount() << " +- "
                      << count_quantity.second.getSingleIncoherentScatterValues().getCountSTD() << std::endl;
            std::cout << "  Multiple scatter: "
                      << count_quantity.second.getMultipleScatterValues().getCount() << " +- "
                      << count_quantity.second.getMultipleScatterValues().getCountSTD() << std::endl << "\n";
        }
        i++;
    }
}



void displaySurfaceTallyResults(const std::vector<std::unique_ptr<SurfaceTally>>& surface_tallies, int N_photons, ComputationalDomain& comp_domain) {
    int i = 1;
    for (auto &tally: surface_tallies) {
        std::cout << "Surface tally " << i << ":" << std::endl;
        std::cout << "----------------" << std::endl;
        auto quantity_container = tally->getSurfaceQuantityContainer();
        auto vector_quantities = quantity_container.getVectorQuantities();
        auto count_quantities = quantity_container.getCountQuantities();
        for (auto &vector_quantity: vector_quantities) {
            std::cout << VectorSurfaceQuantityHelper::toString(vector_quantity.first) << ":" << std::endl;
            std::cout << "  Total: " << vector_quantity.second.getTotalValues().getSum() / N_photons << " +- "
                      << vector_quantity.second.getTotalValues().getSumSTD() / N_photons << std::endl;
            std::cout << "  Primary: " << vector_quantity.second.getPrimaryValues().getSum() / N_photons << " +- "
                      << vector_quantity.second.getPrimaryValues().getSum() / vector_quantity.second.getPrimaryValues().getCountSTD() / N_photons << std::endl;
            std::cout << "  Single Coherent: "
                      << vector_quantity.second.getSingleCoherentScatterValues().getSum() / N_photons << " +- "
                      << vector_quantity.second.getSingleCoherentScatterValues().getSum() / vector_quantity.second.getSingleCoherentScatterValues().getCountSTD() / N_photons << std::endl;
            std::cout << "  Single Incoherent: "
                      << vector_quantity.second.getSingleIncoherentScatterValues().getSum() / N_photons << " +- "
                      << vector_quantity.second.getSingleIncoherentScatterValues().getSumSTD() / N_photons << std::endl;
            std::cout << "  Multiple scatter: "
                      << vector_quantity.second.getMultipleScatterValues().getSum() / N_photons << " +- "
                      << vector_quantity.second.getMultipleScatterValues().getSumSTD() / N_photons << std::endl << "\n";

        }
        for (auto &count_quantity: count_quantities) {
            std::cout << CountSurfaceQuantityHelper::toString(count_quantity.first) << ":" << std::endl;
            std::cout << " Total: " << count_quantity.second.getTotalValues().getCount() << " +- "
                      << count_quantity.second.getTotalValues().getCountSTD() / N_photons << std::endl;
            std::cout << "  Primary: " << count_quantity.second.getPrimaryValues().getCount() << " +- "
                      << count_quantity.second.getPrimaryValues().getCountSTD() / N_photons << std::endl;
            std::cout << "  Single Coherent: "
                      << count_quantity.second.getSingleCoherentScatterValues().getCount() << " +- "
                      << count_quantity.second.getSingleCoherentScatterValues().getCountSTD() / N_photons << std::endl;
            std::cout << "  Single Incoherent: "
                      << count_quantity.second.getSingleIncoherentScatterValues().getCount()  << " +- "
                      << count_quantity.second.getSingleIncoherentScatterValues().getCountSTD()  << std::endl;
            std::cout << "  Multiple scatter: "
                      << count_quantity.second.getMultipleScatterValues().getCount()  << " +- "
                      << count_quantity.second.getMultipleScatterValues().getCountSTD()   << std::endl << "\n";
        }
        i++;
    }
}

void displayVoxelData(ComputationalDomain& comp_domain, int N_photons) {
    const int NUM_VOXEL_GRIDS = comp_domain.getNumVoxelGrids();
    for (int i = 0; i < NUM_VOXEL_GRIDS; i++) {
        std::cout << "Voxel Grid " << i << ":" << std::endl;
        std::cout << "----------------" << std::endl;
        VoxelGrid& voxel_grid = comp_domain.getVoxelGridN(i);
        auto material_deposition = voxel_grid.getEnergyDepositedInMaterials();
        for (auto &material: material_deposition) {
            std::cout << "Material " << material.first << ": " << material.second.first/N_photons << " +- "
            << material.second.second/N_photons << std::endl << "\n";
        }
    }
}

int main() {
    auto data_service = setupDataService();
    auto materials = initializeMaterials();
    auto surface_tallies = initializeSurfaceTallies();
    auto volume_tallies = initializeVolumeTallies();

    InteractionData interaction_data(materials, data_service);
    ComputationalDomain comp_domain("cpp_simulations/CT_w_voxelized_solid/CT_w_voxelized_solid.json");
    PhysicsEngine physics_engine(comp_domain, interaction_data, std::move(volume_tallies), std::move(surface_tallies));

    PhotonSource source = initializeSource();

    const int NUM_OF_PHOTONS = 100000000;

    std::cout << std::fixed << std::setprecision(15);

    runSimulation(source, physics_engine, NUM_OF_PHOTONS);

    displaySurfaceTallyResults(physics_engine.getSurfaceTallies(), NUM_OF_PHOTONS, comp_domain);
    displayVolumeTallyResults(physics_engine.getVolumeTallies(), NUM_OF_PHOTONS, comp_domain);
    displayVoxelData(comp_domain, NUM_OF_PHOTONS);

    return 0;
}
