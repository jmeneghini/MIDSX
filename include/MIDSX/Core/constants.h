#ifndef MCXRAYTRANSPORT_CONSTANTS_H
#define MCXRAYTRANSPORT_CONSTANTS_H

#include <limits>
#include <cmath>

// cgs units (except for energy, which is in eV, because I'm not a complete monster)

const double PI = 3.14159265358979323846;
const double EPSILON = 1e-9;
const double INF = std::numeric_limits<double>::infinity();
const double ELECTRON_REST_MASS = 0.51099895000E6; // eV/c^2
const double ELECTRON_CHARGE = 1.6021766208e-19; // C
const double ELECTRON_RADIUS = 2.8179403227e-13; // cm
const double SPEED_OF_LIGHT = 299792458E2; // cm/s
const double PLANCK_CONSTANT = 4.135667662e-15; // eV*s
const double NUMBER_DENSITY_AT_STP = 0.02504E21; // cm^-3
const double AVOGADRO_CONSTANT = 6.022140857e23; // mol^-1
const double ALPHA = ELECTRON_REST_MASS/(sqrt(2)*PLANCK_CONSTANT*SPEED_OF_LIGHT*1E8); // in angstroms

#endif //MCXRAYTRANSPORT_CONSTANTS_H
