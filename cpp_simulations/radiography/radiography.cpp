#include <MIDSX/Core.h>
#include <pybind11/embed.h>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <omp.h>

namespace py = pybind11;

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
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(0, 0, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(19.5 - 1.5, 0, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(10.5 - 1.5, 10.5 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(19.5 - 1.5, 10.5 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(19.5 - 1.5, 19.5 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(28.5 - 1.5, 28.5 - 1.5, 180),
//            Eigen::Vector3d(3, 0, 0),
//            Eigen::Vector3d(0, 3, 0),
//            surface_container));
//
//    tallies.emplace_back(std::make_unique<RectangularSurfaceTally>(
//            Eigen::Vector3d(39 - 3, 39 - 3, 180),
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
            Eigen::Vector3d(0, 0, 155),
            Eigen::Vector3d(39.0, 39.0, 175),
            volume_container));


    // BODY VOIS:

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 15 - 1.5, 155 + 10 - 1.5),
            Eigen::Vector3d(39.0/2 + 1.5, 39.0/2 - 15 + 1.5, 155 + 10 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(39.0/2 - 15 - 1.5, 39.0/2 - 1.5, 155 + 10 - 1.5),
            Eigen::Vector3d(39.0/2 - 15 + 1.5, 39.0/2 + 1.5, 155 + 10 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 1.5, 155 + 10 - 1.5),
            Eigen::Vector3d(39.0/2 + 1.5, 39.0/2 + 1.5, 155 + 10 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(39.0/2 + 15 - 1.5, 39.0/2 - 1.5, 155 + 10 - 1.5),
            Eigen::Vector3d(39.0/2 + 15 + 1.5, 39.0/2 + 1.5, 155 + 10 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 + 15 - 1.5, 155 + 10 - 1.5),
            Eigen::Vector3d(39.0/2 + 1.5, 39.0/2 + 15 + 1.5, 155 + 10 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 1.5, 155 + 10 - 6 - 1.5),
            Eigen::Vector3d(39.0/2 + 1.5, 39.0/2 + 1.5, 155 + 10 - 6 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 1.5, 155 + 10 - 3 - 1.5),
            Eigen::Vector3d(39.0/2 + 1.5, 39.0/2 + 1.5, 155 + 10 - 3 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 1.5, 155 + 10 + 3 - 1.5),
            Eigen::Vector3d(39.0/2 + 1.5, 39.0/2 + 1.5, 155 + 10 + 3 + 1.5),
            volume_container));

    tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(39.0/2 - 1.5, 39.0/2 - 1.5, 155 + 10 + 6 - 1.5),
            Eigen::Vector3d(39.0/2 + 1.5, 39.0/2 + 1.5, 155 + 10 + 6 + 1.5),
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
            RectangularIsotropicDirectionality(Eigen::Vector3d(0, 0, 180),
                                               Eigen::Vector3d(39.0, 0, 0),
                                               Eigen::Vector3d(0, 39.0, 0)));
    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(PointGeometry(Eigen::Vector3d(39.0/2, 39.0/2, 0)));

    PhotonSource source(std::move(spectrum), std::move(directionality), std::move(geometry));
    return source;
}


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
    auto surface_tallies = initializeSurfaceTallies();
    auto volume_tallies = initializeVolumeTallies();

    ComputationalDomain comp_domain("cpp_simulations/radiography/radiography_0_degrees.json");
    InteractionData interaction_data = comp_domain.getInteractionData();
    PhysicsEngine physics_engine(comp_domain, interaction_data, std::move(volume_tallies), std::move(surface_tallies));

    PhotonSource source = initializeSource();

    const int NUM_OF_PHOTONS = 1000000;

    std::cout << std::fixed << std::setprecision(15);

    runSimulation(source, physics_engine, NUM_OF_PHOTONS);

    displaySurfaceTallyResults(physics_engine.getSurfaceTallies(), NUM_OF_PHOTONS, comp_domain);
    displayVolumeTallyResults(physics_engine.getVolumeTallies(), NUM_OF_PHOTONS, comp_domain);
    displayVoxelData(comp_domain, NUM_OF_PHOTONS);

    return 0;
}
