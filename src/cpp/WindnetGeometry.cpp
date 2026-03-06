/**
 * @file WindnetGeometry.cpp
 * @brief Implementation of canyon geometry classification and windnet placement
 */

#include "WindnetGeometry.hpp"
#include <cmath>
#include <algorithm>

namespace windminer {
namespace geometry {

// Most methods are inline in the header for performance.
// This file reserves space for future complex geometric operations
// or external library integrations (e.g., CGAL for complex footprints).

/**
 * @brief Validate net dimensions against safety constraints
 * 
 * Additional checks beyond header inline validation:
 * - Ensure net does not obstruct emergency vehicle access
 * - Ensure net height allows for standard street lighting clearance
 */
bool Windnet::validateSafetyConstraints() const {
    // Minimum clearance for emergency vehicles (typically 4.5m)
    if (anchor_height_m < 4.5) {
        return false;
    }
    
    // Maximum height to avoid interfering with upper building structures
    if (anchor_height_m + height_m > 15.0) {
        return false;
    }
    
    return true;
}

} // namespace geometry
} // namespace windminer
