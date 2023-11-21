# MIDSX (Monte Carlo Interactions and Dosage Simulation of X-rays)
![Contributors](https://img.shields.io/github/contributors/jmeneghini/MIDSX)
![Forks](https://img.shields.io/github/forks/jmeneghini/MIDSX)
![Stars](https://img.shields.io/github/stars/jmeneghini/MIDSX)
![Issues](https://img.shields.io/github/issues/jmeneghini/MIDSX)
## Description
MIDSX is a code system for simulating the propagation of X-rays through a medium. Using [EPDL](https://www-nds.iaea.org/epics/) and [NIST](https://www.nist.gov/pml/x-ray-mass-attenuation-coefficients) datasets, it samples photon free paths and interactions to propagate photons through a computational domain of specified dimensions and geometries. Geometries/bodies are defined using the [NIfTI-1 Data Format](https://nifti.nimh.nih.gov/nifti-1), which are specified in JSON files. To extract results from a simulation, both Volume and Surface tallies with specifiable measurable quantities and geometries are available, along with derived quantities, such as air kerma for HVL measurements. The following project is a **WIP**, so documentation is currently not available, but it is in the works.

## Getting Started

### Dependencies

MIDSX requires the following dependencies to be manually installed:

* **CMake 3.10.0 or higher:** If you don't have CMake install, or require a newer version, follow this [guide](https://askubuntu.com/questions/355565/how-do-i-install-the-latest-version-of-cmake-from-the-command-line)

* **SQLite3 Library:** On Linux, the library can be installed using your distribution's package manager. Using apt: `sudo apt install sqlite3` \
On Mac, the library can be installed with brew: `brew install sqlite3`

* **Python 3.8.x or higher:** If not already installed, go [here](https://www.python.org/downloads/)

* **nibabel:** In order to load NifTI files, MIDSX uses the python package nibabel. It can be easily installed with pip: `pip install nibabel`

MIDSX additionally uses the following libraries via Git submodules; these do not need to be installed manually:

* **Eigen:** For data storage and linear algebra

* **pybind11:** For use of nibabel in C++ code. Will be used later for MIDSX python bindings

### Installation

MIDSX has been tested on MacOS and Ubuntu. To install with the command line:

1. Clone the repo and enter the directory
 ```sh
 git clone https://github.com/jmeneghini/MIDSX.git
 cd MIDSX
 ```

2. Create and enter the build directory
```sh
mdir build
cd build
```

3. Generate cmake files and install
```sh
cmake ..
sudo make install
```

## Usage
To use the library, configure a project with the following CMakeLists.txt structure:

```cmake
cmake_minimum_required(VERSION 3.10)
project(project_name)

# needed for parallelization
find_package(OpenMP REQUIRED)

# links libraries and turns off parallellization if building in debug mode
function(create_executable EXE_NAME SRC_FILE)
    add_executable(${EXE_NAME} ${SRC_FILE})
    target_link_libraries(${EXE_NAME} PRIVATE ${COMMON_LIBS})
    if(CMAKE_BUILD_TYPE MATCHES Release)
        message(STATUS "Release build. Parallelization enabled.")
        target_link_libraries(${EXE_NAME} PUBLIC OpenMP::OpenMP_CXX)
    endif()
endfunction()

# finds pybind11 (doesn't seem to link with library) and MIDSX
find_package(pybind11 REQUIRED)
find_package(MIDSX REQUIRED)

# sets common libs that are linked to executable
set(COMMON_LIBS pybind11::embed MIDSX::MIDSX)

create_executable(project main.cpp)
```

A typical MIDSX simulation has the following structure:

* A .json file containing scene information:
```json
{
  "dim_space": [
    4,
    4,
    100
  ],
  "background_material_id": 3,
  "voxel_grids": [
    {
      "file_path": "path/to/nifti/file.nii",
      "origin": [0, 0, 10]
    }
  ]
}
```

* An `DataAccessObject` which is used to access the SQLite database. By default, this is located
at `MIDSX/data/data_sources/EPDL/EPDL.db`.
```C++
#include <MIDSX/Core.h>
data_service = DataAccessObject("data/data_sources/EPDL/EPDL.db")
```

* With `DataAccessObject` and a vector of material names, the `InteractionData` object can be constructed. For all the available materials, see the SQLite database. I recommend [DB Browser for SQLite](https://sqlitebrowser.org/).
```C++
std::vector<std::string> materials = {"Air - C5", "Heart - C5"};
InteractionData interaction_data(materials, data_service);
```
* Using the .json file, the `ComputationalDomain` object can be initialized.
```C++
ComputationalDomain comp_domain("domain.json")
```

* In order to make measurements, tallies must be specified. Various `SurfaceTally` and `VolumeTally` objects are available, and these objects must be supplied a `QuanityContainer` which holds `Quantity`'s to be measured by the tallies. These containers can be created manually, or predefined containers can be used via `QuanitityContainerFactory`. These created tallies must be contained as pointers in two separate vectors for surface and volume tallies.

```C++
std::vector<std::unique_ptr<SurfaceTally>> surface_tallies = {}
tallies.emplace_back(std::make_unique<DiscSurfaceTally>(
            Eigen::Vector3d(2, 2, 100),
            Eigen::Vector3d(0, 0, 1),
            1.0,
            SurfaceQuantityContainerFactory::AllQuantities()));
    return tallies;

std::vector<std::unique_ptr<VolumeTally>> volume_tallies = {};
auto volume_container = VolumeQuantityContainer();
volume_container.addVectorQuantity(VectorVolumeQuantity(VectorVolumeQuantityType::EnergyDeposition));
tallies.emplace_back(std::make_unique<AACuboidVolumeTally>(
            Eigen::Vector3d(0, 0, 155),
            Eigen::Vector3d(39.0, 39.0, 175),
            volume_container));
```

* Using all these objects, the `PhysicsEngine` can now be initialized.
```C++
PhysicsEngine physics_engine(comp_domain, interaction_data,
 std::move(volume_tallies), std::move(surface_tallies));
```

* In order to run the simulation, one just needs to a way to generate photons. MIDSX uses `EnergySpectrum`, `Directionality`, and `SourceGeometry` objects to build a `PhotonSource`.

```C++
PolyenergeticSpectrum poly_spectrum(energy_spectrum);
   MonoenergeticSpectrum mono_spectrum(56.4E3);
   std::unique_ptr<EnergySpectrum> spectrum = std::make_unique<MonoenergeticSpectrum>(mono_spectrum);

   std::unique_ptr<Directionality> directionality = std::make_unique<BeamDirectionality>(BeamDirectionality(Eigen::Vector3d(39.0/2, 39.0/2, 180)));


    std::unique_ptr<SourceGeometry> geometry = std::make_unique<PointGeometry>(PointGeometry(Eigen::Vector3d(0.0, dim_space.y()/2.0, dim_space.z()/2.0)));

    PhotonSource source(std::move(spectrum), std::move(directionality), std::move(geometry));
```

* Now, with `PhotonSource` and `PhysicsEngine`, you can now run a simulation in parallel.

```C++
int j = 0;
N_photons = 1E6;
#pragma omp parallel for
for (int i = 0; i < N_photons; i++) {
    Photon photon = source.generatePhoton();
    physics_engine.transportPhoton(photon);
    if (i % (N_photons / 20) == 0) {
        std::cout << "Progress: " << j << "%" << std::flush << "\r";
        j += 5;
    }
}
```

* Data can be retrieved from the simulation via `physics_engine.getSurfaceTallies()` and `physics_engine.getVolumeTallies()`.

* For further info, look at the several examples in the `cpp_simulations` folder.







