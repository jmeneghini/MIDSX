#ifndef HELPER_H
#define HELPER_H

#include <cmath>
#include <vector>
#include <sstream>
#include <string>
#include <map>
#include <limits>
#include <iostream>
#include <algorithm>


// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degreesToRadians(double degrees) {
    return degrees * pi / 180.0;
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
inline std::vector<T> convertStringVector(const std::vector<std::string>& stringVector) {
    // Create a vector of the new type
    std::vector<T> convertedVector(stringVector.size());

    // Convert each string to the new type
    std::transform(stringVector.begin(), stringVector.end(), convertedVector.begin(),
                   [](const std::string& str) {
                       std::istringstream iss(str);
                       T value;
                       iss >> value;
                       return value;
                   }
    );

    return convertedVector;
}

template <typename T>
std::vector<std::vector<T>> distributeNTimes(const std::vector<T>& source, int N) {
    std::vector<std::vector<T>> result(N);  // Initialize vector of vectors

    for (int i = 0; i < source.size(); ++i) {
        result[i % N].push_back(source[i]);
    }

    return result;
}


#endif // HELPER_H