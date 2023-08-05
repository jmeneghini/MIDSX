#include "source.h"
#include "pybind11/embed.h"
#include "interaction_data.h"
#include "probability_dist.h"
#include <matplot/matplot.h>
#include "voxel_grid.h"
#include "physics_engine.h"
#include "detector.h"
#include "source.h"


namespace plt = matplot;
namespace py = pybind11;

int main() {
    auto data_service = std::make_shared<DataAccessObject>("data/data_sources/EPDL/EPDL.db");
    std::vector<std::shared_ptr<Material>> materials;
    materials.emplace_back(std::make_shared<Material>(Material("Air, Dry (near sea level)", data_service)));
    materials.emplace_back(std::make_shared<Material>(Material("Tissue, Soft (ICRU-44)", data_service)));
    InteractionData interaction_data(materials, data_service);
    VoxelGrid voxel_grid("data/voxels/tissue_dose_test_15_degrees.nii.gz");
    Eigen::Vector3d detector_position(5, 5, 100);
    Detector detector(detector_position); // creates a point detector
    PhysicsEngine physics_engine(voxel_grid, interaction_data, detector);

    std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<MonoenergeticSpectrum>
            (MonoenergeticSpectrum(56.4E3));


    Eigen::Vector3d voxel_origin_to_max = voxel_grid.getDimSpace();
    std::unique_ptr<Directionality> directionality = std::make_unique<RectangularIsotropicDirectionality>
            (RectangularIsotropicDirectionality(voxel_grid.getOrigin() + Eigen::Vector3d(7, 0, voxel_origin_to_max.z()),
                                                voxel_grid.getOrigin() + voxel_origin_to_max));
    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>


            (PointGeometry(Eigen::Vector3d(0, 19.5, 0)));
    PhotonSource source(std::move(spectrum), std::move(directionality), std::move(geometry));

    int N_photons = 1000000;
    int j = 0;
    for (int i=0; i<N_photons; i++) {
        Photon photon = source.generatePhoton();
        physics_engine.transportPhoton(photon);
        if (i % (N_photons/20) == 0) {
            std::cout << "Progress: " << j << "%" << std::flush << "\r";
            j += 5;
        }

    }
    DetectorTallies detector_tallies = detector.getTallies();
    std::cout << "Detector tallies: " << std::endl;
    std::cout << "Total number of photons detected: " << detector_tallies.total_photons_hit << std::endl;
    std::cout << "Total energy deposited: " << detector_tallies.total_energy_deposited << std::endl;
    std::cout << "Number of primary photons detected: " << detector_tallies.primary_photons_hit << std::endl;
    std::cout << "Energy deposited by primary photons: " << detector_tallies.energy_deposited_by_primary_photons << std::endl;
    std::cout << "Number of secondary photons detected: " << detector_tallies.secondary_photons_hit << std::endl;
    std::cout << "Energy deposited by secondary photons: " << detector_tallies.energy_deposited_by_secondary_photons << std::endl;
    std::cout << "Energy Deposited in Voxel Grid (ev/photon): " << voxel_grid.getTotalEnergyDeposited() / N_photons << std::endl;
    std::cout << "Energy Deposited in Voxel Grid per material (ev/photon): " << std::endl;
    auto materials_dose = voxel_grid.getEnergyDepositedInMaterials();
    for (auto& material_dose : materials_dose) {
        std::cout << material_dose.first << ": " << material_dose.second / N_photons << std::endl;
    }
    return 0;
}