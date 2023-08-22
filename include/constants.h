#ifndef MCXRAYTRANSPORT_CONSTANTS_H
#define MCXRAYTRANSPORT_CONSTANTS_H

#include <limits>

// cgs units (except for energy, which is in eV, because I'm not a complete monster)

const double PI = 3.14159265358979323846;
const double ELECTRON_REST_MASS = 0.51099895000E6; // eV/c^2
const double ELECTRON_CHARGE = 1.6021766208e-19; // C
const double ELECTRON_RADIUS = 2.8179403227e-13; // cm
const double SPEED_OF_LIGHT = 299792458E2; // cm/s
const double PLANCK_CONSTANT = 4.135667662e-15; // eV*s
const double NUMBER_DENSITY_AT_STP = 0.02504E21; // cm^-3

#endif //MCXRAYTRANSPORT_CONSTANTS_H
