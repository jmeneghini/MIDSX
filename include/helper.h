#ifndef HELPER_H
#define HELPER_H

#include <cmath>
#include <vector>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degreesToRadians(double degrees) {
    return degrees * pi / 180.0;
}

#endif // HELPER_H