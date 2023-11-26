#include <MIDSX/Core.h>
#include <pybind11/embed.h>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <omp.h>

namespace py = pybind11;

Eigen::Vector3d body_origin(28.730854637602084, 28.730854637602084, 155.0);
Eigen::Vector3d dim_space(96.46170927520417, 96.46170927520417, 180.0);

std::vector<std::string> initializeMaterials() {
    std::vector<std::string> materials = {};
    materials.emplace_back("Tissue, Soft (ICRU-46)");
//    materials.emplace_back("Al");
    materials.emplace_back("Air, Dry (near sea level)");
    return materials;
}

std::vector<std::unique_ptr<SurfaceTally>> initializeSurfaceTallies() {
    std::vector<std::unique_ptr<SurfaceTally>> tallies = {};

    auto surface_container = SurfaceQuantityContainer();
    surface_container.addVectorQuantity(VectorSurfaceQuantity(VectorSurfaceQuantityType::IncidentEnergy));

    // TEST
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 1.5, 0),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            std::move(SurfaceQuantityContainerFactory::AllQuantities())));
//

    // FULL FIELD ROIS:

//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            body_origin + Eigen::Vector3d(0, 0, 25),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            body_origin + Eigen::Vector3d(19.5 - 1.5, 0, 25),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            body_origin + Eigen::Vector3d(10.5 - 1.5, 10.5 - 1.5, 25),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            body_origin + Eigen::Vector3d(19.5 - 1.5, 10.5 - 1.5, 25),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            body_origin + Eigen::Vector3d(19.5 - 1.5, 19.5 - 1.5, 25),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            body_origin + Eigen::Vector3d(28.5 - 1.5, 28.5 - 1.5, 25),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            body_origin + Eigen::Vector3d(39 - 3, 39 - 3, 25),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));



// PENCIL BEAM ROIS:

//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(39.0/2 - 6 - 1.5, 39.0/2 - 6 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 6 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(39.0/2 - 3 - 1.5, 39.0/2 - 3 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 3 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(39.0/2 + 3 - 1.5, 39.0/2 + 3 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(39.0/2 + 6 - 1.5, 39.0/2 + 6 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));

    return tallies;
}

std::vector<std::unique_ptr<VolumeTally>> initializeVolumeTallies() {
    std::vector<std::unique_ptr<VolumeTally>> tallies = {};

    auto volume_container = VolumeQuantityContainer();
    volume_container.addVectorQuantity(VectorVolumeQuantity(VectorVolumeQuantityType::EnergyDeposition));

    // WHOLE BODY VOI:

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin,
            body_origin + Eigen::Vector3d(39.0, 39.0, 20.0),
            volume_container));


    // BODY VOIS:

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin + Eigen::Vector3d(39.0/2.0 - 1.5, 39.0/2.0 - 15.0 - 1.5, 10.0 - 1.5),
            body_origin + Eigen::Vector3d(39.0/2.0 + 1.5, 39.0/2.0 - 15.0 + 1.5, 10.0 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin + Eigen::Vector3d(39.0/2.0 - 15.0 - 1.5, 39.0/2.0 - 1.5, 10.0 - 1.5),
            body_origin + Eigen::Vector3d(39.0/2.0 - 15.0 + 1.5, 39.0/2.0 + 1.5, 10.0 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin + Eigen::Vector3d(39.0/2.0 - 1.5, 39.0/2.0 - 1.5, 10.0 - 1.5),
            body_origin + Eigen::Vector3d(39.0/2.0 + 1.5, 39.0/2.0 + 1.5, 10.0 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin + Eigen::Vector3d(39.0/2.0 + 15.0 - 1.5, 39.0/2.0 - 1.5, 10.0 - 1.5),
            body_origin + Eigen::Vector3d(39.0/2.0 + 15.0 + 1.5, 39.0/2.0 + 1.5, 10.0 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin + Eigen::Vector3d(39.0/2.0 - 1.5, 39.0/2.0 + 15.0 - 1.5, 10.0 - 1.5),
            body_origin + Eigen::Vector3d(39.0/2.0 + 1.5, 39.0/2.0 + 15.0 + 1.5, 10.0 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin + Eigen::Vector3d(39.0/2.0 - 1.5, 39.0/2.0 - 1.5, 10.0 - 6.0 - 1.5),
            body_origin + Eigen::Vector3d(39.0/2.0 + 1.5, 39.0/2.0 + 1.5, 10.0 - 6.0 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin + Eigen::Vector3d(39.0/2.0 - 1.5, 39.0/2.0 - 1.5, 10.0 - 3.0 - 1.5),
            body_origin + Eigen::Vector3d(39.0/2.0 + 1.5, 39.0/2.0 + 1.5, 10.0 - 3.0 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin + Eigen::Vector3d(39.0/2.0 - 1.5, 39.0/2.0 - 1.5, 10.0 + 3.0 - 1.5),
            body_origin + Eigen::Vector3d(39.0/2.0 + 1.5, 39.0/2.0 + 1.5, 10.0 + 3.0 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            body_origin + Eigen::Vector3d(39.0/2.0 - 1.5, 39.0/2.0 - 1.5, 10.0 + 6.0 - 1.5),
            body_origin + Eigen::Vector3d(39.0/2.0 + 1.5, 39.0/2.0 + 1.5, 10.0 + 6.0 + 1.5),
            volume_container));

    return tallies;
}

Eigen::MatrixXd processEnergySpectrum() {
    Eigen::MatrixXd energy_spectrum = SourceHelpers::readCSV("data/source_distributions/RQR9_W_AL_120kVp_E_spectrum.csv");
    energy_spectrum.col(0) = energy_spectrum.col(0).array() * 1000;  // Convert from keV to eV
    return energy_spectrum;
}

PhotonSource initializeSource() {
    auto energy_spectrum = processEnergySpectrum();

    PolyenergeticSpectrum poly_spectrum(energy_spectrum);
    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<PolyenergeticSpectrum>(poly_spectrum);
//    MonoenergeticSpectrum mono_spectrum(56.4E3);
//    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<MonoenergeticSpectrum>(mono_spectrum);
//
//    std::unique_ptr<Directionality> directionality = std::make_unique<BeamDirectionality>(BeamDirectionality(Eigen::Vector3d(
//            39.0/2, 39.0/2, 180)));
    std::unique_ptr<Directionality> directionality = std::make_unique<RectangularIsotropicDirectionality>(
            RectangularIsotropicDirectionality(body_origin + Eigen::Vector3d(0.0, 0.0, 25.0),
                                               Eigen::Vector3d(39.0, 0.0, 0.0),
                                               Eigen::Vector3d(0.0, 39.0, 0.0)));
    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(
            PointGeometry(Eigen::Vector3d(dim_space.x()/2.0, 0.0, 0.0)));

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
        auto& quantity_container = tally->getVolumeQuantityContainer();
        auto& vector_quantities = quantity_container.getVectorQuantities();
        auto& count_quantities = quantity_container.getCountQuantities();
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
                      << vector_quantity.second.getMultipleScatterValues().getSumSTD() / N_photons << std::endl;
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
                      << count_quantity.second.getMultipleScatterValues().getCountSTD() << std::endl;
        }
        i++;
    }
}



void displaySurfaceTallyResults(const std::vector<std::unique_ptr<SurfaceTally>>& surface_tallies, int N_photons, ComputationalDomain& comp_domain) {
    int i = 1;
    for (auto &tally: surface_tallies) {
        std::cout << "Surface tally " << i << ":" << std::endl;
        std::cout << "----------------" << std::endl;
        auto& quantity_container = tally->getSurfaceQuantityContainer();
        auto& vector_quantities = quantity_container.getVectorQuantities();
        auto& count_quantities = quantity_container.getCountQuantities();
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
            std::cout << "Material " << material.first << ": " << material.second/N_photons << std::endl << "\n";
        }
    }
}

int main() {
    auto materials = initializeMaterials();
    auto surface_tallies = initializeSurfaceTallies();
    auto volume_tallies = initializeVolumeTallies();

    InteractionData interaction_data(materials);
    ComputationalDomain comp_domain("cpp_simulations/radiography/radiography_15_degrees.json");
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
