#ifndef VOXEL_H
#define VOXEL_H

#include <string>
#include "quantity.h"

/**
 * @brief Struct which represents a voxel.
 *
 * Stores the material ID and dose for a voxel.
 */
struct Voxel {
    uint8_t materialID = 0;
    VectorValue dose;
};

#endif // VOXEL_H