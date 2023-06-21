#ifndef HELPER_H
#define HELPER_H

#include <cmath>
#include <vector>
#include <sstream>
#include <string>
#include <map>
#include <limits>
#include <iostream>


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

#endif // HELPER_H