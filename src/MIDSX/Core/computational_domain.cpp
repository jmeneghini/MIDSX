#include "Core/computational_domain.h"

using json = nlohmann::json;

ComputationalDomain::ComputationalDomain(const std::string &json_file_path,
                                         bool is_python_environment):
                                         is_python_environment_(is_python_environment) {
    initializeCompDomain(json_file_path);
}

InteractionData ComputationalDomain::getInteractionData() const {
    std::vector<std::string> material_names = getMaterialNames();
    return InteractionData(material_names);
}

bool ComputationalDomain::isInComputationalDomain(const Eigen::Vector3d &position) const {
    return position.x() >= 0 && position.x() <= dim_space_.x() &&
           position.y() >= 0 && position.y() <= dim_space_.y() &&
           position.z() >= 0 && position.z() <= dim_space_.z();

}

Voxel& ComputationalDomain::getVoxel(const Eigen::Vector3d &position) {
    for (auto &voxel_grid : voxel_grids_) {
        Eigen::Vector3d position_in_voxel_grid = position - voxel_grid.second;
        if (voxel_grid.first.withinGrid(position_in_voxel_grid)) {
            return voxel_grid.first.getVoxel(voxel_grid.first.getVoxelIndex(position_in_voxel_grid));
        }
    }
    return background_voxel;
}

VoxelGrid& ComputationalDomain::getVoxelGridN(int N) {
    return voxel_grids_[N].first;
}

Eigen::Vector3d ComputationalDomain::getVoxelGridOriginN(int N) {
    return voxel_grids_[N].second;
}

Eigen::Vector3d ComputationalDomain::getVoxelGridDimSpaceN(int N) {
    return voxel_grids_[N].first.getDimSpace();
}

int ComputationalDomain::getNumVoxelGrids() const {
    return voxel_grids_.size();
}


void ComputationalDomain::initializeCompDomain(const std::string &json_file_path) {
    json json_object;
    std::string json_directory_path = std::filesystem::path(json_file_path).parent_path();
    if (isJSON(json_file_path)) {
        // create json object
        std::ifstream json_file(json_file_path);
        json_file >> json_object;
        setCompProperties(json_object);
        setVoxelGrids(json_object, json_directory_path);
    }
    else {
        throw std::runtime_error("The file provided is not a JSON file.");
    }
}

bool ComputationalDomain::isJSON(const std::string &file_path) {
    return file_path.find(".json") != std::string::npos;
}

void ComputationalDomain::setCompProperties(const json &json_object) {
    std::array<double, 3> dim_space = json_object["dim_space"];
    dim_space_ = Eigen::Map<Eigen::Vector3d>(dim_space.data());
    // set background voxel
    // initialize dummy interaction data object to use helper function
    InteractionData interaction_data({});
    background_voxel.materialID = interaction_data.getAnyMaterialIdFromName(json_object["background_material_name"]);
}

void ComputationalDomain::setVoxelGrids(const json &json_object, const std::string &json_directory_path) {
    std::vector<std::string> nifti_file_paths;
    std::vector<Eigen::Vector3d> origins;
    for (auto &voxel_grid_json: json_object["voxel_grids"]) {
        getNIFTIFilePaths(voxel_grid_json, json_directory_path, nifti_file_paths);
        getOrigins(voxel_grid_json, origins);
    }
    for (int i = 0; i < nifti_file_paths.size(); i++) {
        voxel_grids_.emplace_back(VoxelGrid(nifti_file_paths[i], is_python_environment_), origins[i]);
    }
}

std::vector<std::string> ComputationalDomain::getMaterialNames() const {
    std::vector<std::string> material_names;
    // get background material name
    material_names.push_back(getBackgroundMaterialName());
    // get unique material names from voxel grids
    for (auto& voxel_grid : voxel_grids_) {
        std::vector<std::string> voxel_grid_material_names = voxel_grid.first.getMaterialNames();
        for (auto& material_name : voxel_grid_material_names) {
            if (std::find(material_names.begin(), material_names.end(), material_name) == material_names.end()) {
                material_names.push_back(material_name);
            }
        }
    }
    return material_names;
}

std::string ComputationalDomain::getBackgroundMaterialName() const {
    // create dummy interaction data object to use helper function
    InteractionData interaction_data({});
    int background_material_id = background_voxel.materialID;
    return interaction_data.getAnyMaterialNameFromId(background_material_id);
}


void ComputationalDomain::getNIFTIFilePaths(const json& voxel_grid_json, const std::string &json_directory_path,
                                            std::vector<std::string> &nifti_file_paths) {
    std::string file_path = voxel_grid_json["file_path"];
    std::filesystem::path path = file_path;
    if (isNIFTI(file_path)) {
        if (path.is_absolute()) {
            nifti_file_paths.push_back(file_path);
        }
        else {
            nifti_file_paths.push_back(json_directory_path + "/" + file_path);
        }
    }
    else {
        throw std::runtime_error("The file provided is not a NIFTI file.");
    }
}

void ComputationalDomain::getOrigins(const json &voxel_grid_json, std::vector<Eigen::Vector3d> &origins) {
    std::array<double, 3> origin = voxel_grid_json["origin"];
    Eigen::Vector3d origin_vector = Eigen::Map<Eigen::Vector3d>(origin.data());
    origins.push_back(origin_vector);
}


bool ComputationalDomain::isNIFTI(const std::string &file_path) {
    return file_path.find(".nii") != std::string::npos;
}


