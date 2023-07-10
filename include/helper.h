#ifndef HELPER_H
#define HELPER_H

#include <cmath>
#include <vector>
#include <sstream>
#include <string>
#include <map>
#include <functional>
#include <iostream>
#include <algorithm>
#include "constants.h"
#include <valarray>
#include <Eigen/Dense>
#include <fstream>


// Utility Functions

using Vector3D = Eigen::Vector3d;

inline double degreesToRadians(double degrees) {
    return degrees * PI / 180.0;
}

inline std::vector<double> stringToVector(const std::string& s) {
    if (s.empty() || s.front() != '[' || s.back() != ']') {
        throw std::invalid_argument("Input string is not a valid JSON array");
    }

    std::vector<double> result;
    std::stringstream ss(s);
    std::string item;

    // skip the opening '['
    ss.ignore(1);

    while (std::getline(ss, item, ',')) {
        try {
            result.push_back(std::stod(item));
        } catch (const std::invalid_argument& e) {
            throw std::invalid_argument("Invalid numeric value in input string");
        }
    }
    return result;
}






// Convert Eigen::VectorXd to std::vector
template <typename T>
std::vector<T> convertEigenVectorToStdVector(const Eigen::Matrix<T, Eigen::Dynamic, 1>& source) {
    std::vector<T> result(source.data(), source.data() + source.size());
    return result;
}

// Eigen logspace function
template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, 1> logspace(int num, T start, T end) {
    Eigen::Matrix<T, Eigen::Dynamic, 1> result(num);
    T delta = (end - start) / (num - 1);
    for (int i = 0; i < num; ++i) {
        result(i) = pow(10, start + delta * i);
    }
    return result;
}

template<typename T>
T load_csv (const std::string& path) {
    std::ifstream in_data;
    in_data.open(path);
    std::string line;
    std::vector<double> values;
    uint rows = 0;
    while (std::getline(in_data, line)) {
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, ',')) {
            values.push_back(std::stod(cell));
        }
        ++rows;
    }
    return Eigen::Map<const Eigen::Matrix<typename T::Scalar, T::RowsAtCompileTime, T::ColsAtCompileTime, Eigen::RowMajor>>(values.data(), rows, values.size()/rows);
}


#endif // HELPER_H
