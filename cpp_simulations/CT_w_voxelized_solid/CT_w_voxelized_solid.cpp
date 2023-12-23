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

void displayVolumeTallyResults(std::vector<VolumeQuantityContainer>& quantity_containers, int N_photons, ComputationalDomain& comp_domain) {
    int i = 1;
    for (auto &quantity_container: quantity_containers) {
        std::cout << "Volume tally " << i << ":" << std::endl;
        std::cout << "----------------" << std::endl;
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

void displaySurfaceTallyResults(std::vector<SurfaceQuantityContainer>& quantity_containers, int N_photons, ComputationalDomain& comp_domain) {
    int i = 1;
    for (auto &quantity_container: quantity_containers) {
        std::cout << "Surface tally " << i << ":" << std::endl;
        std::cout << "----------------" << std::endl;
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
    ComputationalDomain comp_domain("cpp_simulations/CT_w_voxelized_solid/CT_w_voxelized_solid.json");
    InteractionData interaction_data = comp_domain.getInteractionData();
    PhysicsEngine physics_engine(comp_domain, interaction_data);

    PhotonSource source = initializeSource();

    const int NUM_OF_PHOTONS = 100000000;

    std::cout << std::fixed << std::setprecision(15);

    runSimulation(source, physics_engine, initializeSurfaceTallies, initializeVolumeTallies, NUM_OF_PHOTONS);

    auto surface_tally_results = physics_engine.getSurfaceQuantityContainers();
    auto volume_tally_results = physics_engine.getVolumeQuantityContainers();

    displaySurfaceTallyResults(surface_tally_results, NUM_OF_PHOTONS, comp_domain);
    displayVolumeTallyResults(volume_tally_results, NUM_OF_PHOTONS, comp_domain);
    displayVoxelData(comp_domain, NUM_OF_PHOTONS);


    return 0;
}
