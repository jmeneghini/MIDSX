#ifndef HVL_COMPUTATIONAL_DOMAIN_H
#define HVL_COMPUTATIONAL_DOMAIN_H

#include "voxel_grid.h"
#include "json.h"
#include <vector>
#include <Eigen/Core>
#include <filesystem>
#include <fstream>

using json = nlohmann::json;

class ComputationalDomain {
public:
    ComputationalDomain() = default;
    explicit ComputationalDomain(const std::string &json_file_path);

    bool isInComputationalDomain(const Eigen::Vector3d &position) const;
    Voxel& getVoxel(const Eigen::Vector3d &position);

    Voxel background_voxel;
private:
    std::vector<std::pair<VoxelGrid, Eigen::Vector3d>> voxel_grids_;
    Eigen::Vector3d dim_space_;

    // related public functions


    // initialization
    void initializeCompDomain(const std::string &json_file_path);
    static bool isJSON(const std::string &file_path);
    void setCompProperties(const json &json_object);
    void setVoxelGrids(const json &json_object, const std::string &json_directory_path);

    static void getNIFTIFilePaths(const json &voxel_grid_json, const std::string &json_directory_path, std::vector<std::string> &nifti_file_paths);
    static void getOrigins(const json &voxel_grid_json, std::vector<Eigen::Vector3d> &origins);

    static bool isNIFTI(const std::string &file_path);
};

#endif //HVL_COMPUTATIONAL_DOMAIN_H
