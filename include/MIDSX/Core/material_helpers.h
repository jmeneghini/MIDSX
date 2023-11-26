#ifndef MCXRAYTRANSPORT_MATERIAL_HELPERS_H
#define MCXRAYTRANSPORT_MATERIAL_HELPERS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <Eigen/Dense>

namespace MaterialHelpers {

    template <typename T, typename U>
    std::vector<T> mapKeysToVector(const std::unordered_map<T, U>& map) {
        std::vector<T> keys;
        for (const auto& pair : map) {
            keys.push_back(pair.first);
        }
        return keys;
    }

    template <typename T, typename U>
    std::vector<U> mapElementsToVector(const std::unordered_map<T, U>& map) {
        std::vector<U> elements;
        for (const auto& pair : map) {
            elements.push_back(pair.second);
        }
        return elements;
    }

    template <typename T, typename U>
    std::string mapKeysToString(const std::unordered_map<T, U>& map) {
        std::stringstream ss;
        for (auto itr = map.begin(); itr != map.end(); ++itr) {
            ss << itr->first;
            if (std::next(itr) != map.end())
                ss << ", ";
        }
        return ss.str();
    }

    inline double calculateWeightedAverage(const std::vector<double>& weights, const std::vector<double>& values) {
        double weighted_average = 0.0;
        for (std::size_t i = 0; i < weights.size(); ++i) {
            weighted_average += weights[i] * values[i];
        }
        return weighted_average;
    }
}

namespace InteractionDataHelpers {

    template<typename T>
    inline std::vector<T> castStringVector(const std::vector<std::string> &string_vector) {
        // Create a vector of the new type
        std::vector<T> converted_vector(string_vector.size());

        // Convert each string to the new type
        std::transform(string_vector.begin(), string_vector.end(), converted_vector.begin(),
                       [](const std::string &str) {
                           std::istringstream iss(str);
                           T value;
                           iss >> value;
                           return value;
                       }
        );

        return converted_vector;
    }

    inline Eigen::MatrixXd mergeMatrices(std::vector<Eigen::MatrixXd> &matrices) {
        std::vector<double> merged;

        for (const auto &matrix: matrices) {
            std::vector<double> v(matrix.data(), matrix.data() + matrix.size());
            merged.insert(merged.end(), v.begin(), v.end());
        }

        std::sort(merged.begin(), merged.end());

        auto last = std::unique(merged.begin(), merged.end());
        merged.erase(last, merged.end());

        Eigen::MatrixXd result = Eigen::Map<Eigen::MatrixXd>(merged.data(), merged.size(), 1);

        return result;
    }

    template<typename T>
    Eigen::MatrixXd convertNVectorsToEigenMatrix(const std::vector<std::vector<T>> &source) {
        Eigen::MatrixXd result(source[0].size(), source.size());

        for (std::size_t i = 0; i < source.size(); ++i) {
            result.col(i) = Eigen::Map<const Eigen::VectorXd>(source[i].data(), source[i].size());
        }

        return result;
    }

    template<typename T>
    std::vector<std::vector<T>> distributeNTimes(const std::vector<T> &source, int n) {
        std::vector<std::vector<T>> result(n);  // Initialize vector of vectors

        for (std::size_t i = 0; i < source.size(); ++i) {
            result[i % n].push_back(source[i]);
        }

        return result;
    }

    // convert material id to material name
    inline std::string convertMaterialIdToName(int material_id, DataAccessObject &dao) {
        // Add each element of the vector to a string if there are multiple elements
        // (might be split into multiple lines)
        std::string query = "SELECT Name FROM Materials WHERE MaterialID = " + std::to_string(material_id) + ";";
        return dao.executeQuery(query)[0];
    }
}

#endif //MCXRAYTRANSPORT_MATERIAL_HELPERS_H