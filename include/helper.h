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

template <typename T>
inline std::vector<T> convertStringVector(const std::vector<std::string>& string_vector) {
    // Create a vector of the new type
    std::vector<T> converted_vector(string_vector.size());

    // Convert each string to the new type
    std::transform(string_vector.begin(), string_vector.end(), converted_vector.begin(),
                   [](const std::string& str) {
                       std::istringstream iss(str);
                       T value;
                       iss >> value;
                       return value;
                   }
    );

    return converted_vector;
}

template <typename T>
std::vector<std::vector<T>> distributeNTimes(const std::vector<T>& source, int n) {
    std::vector<std::vector<T>> result(n);  // Initialize vector of vectors

    for (int i = 0; i < source.size(); ++i) {
        result[i % n].push_back(source[i]);
    }

    return result;
}

// Convert N sets of vectors to an N-dimensional Eigen::MatrixXd
template <typename T>
Eigen::MatrixXd convertNVectorsToEigenMatrix(const std::vector<std::vector<T>>& source) {
    Eigen::MatrixXd result(source[0].size(), source.size());

    for (int i = 0; i < source.size(); ++i) {
        result.col(i) = Eigen::Map<const Eigen::VectorXd>(source[i].data(), source[i].size());
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

double linearInterpolation(const Eigen::MatrixXd& data, double x) {
    // If x is out of range, extrapolate
    if (x <= data(0, 0)) {
        return data(0, 1);
    } else if (x >= data(data.rows() - 1, 0)) {
        return data(data.rows() - 1, 1);
    } else {
        // Find the index of the first value in the data that is greater than x
        int i = 0;
        while (data(i, 0) < x) {
            ++i;
        }

        // Linearly interpolate between the two values
        double x1 = data(i - 1, 0);
        double x2 = data(i, 0);
        double y1 = data(i - 1, 1);
        double y2 = data(i, 1);
        return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
    }
}





#endif // HELPER_H
