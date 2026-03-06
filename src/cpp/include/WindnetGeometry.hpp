/**
 * @file WindnetGeometry.hpp
 * @brief Canyon geometry classification and windnet placement calculations
 * 
 * This module handles urban street canyon geometric analysis, including:
 * - Aspect ratio (h/w) calculation
 * - Flow regime classification (skimming, wake-interference, isolated)
 * - Optimal net placement zones based on vortex dynamics
 * 
 * @author Ecotribute Research Team
 * @date 2026
 */

#ifndef WINDNET_GEOMETRY_HPP
#define WINDNET_GEOMETRY_HPP

#include <string>
#include <vector>
#include <cmath>

namespace windminer {
namespace geometry {

/**
 * @brief Flow regime classification based on canyon aspect ratio
 * 
 * Reference: Oke (1988), Street Canyon Flow Regimes
 */
enum class FlowRegime {
    ISOLATED_ROUGHNESS,    ///< h/w < 0.3 - Buildings act as individual obstacles
    WAKE_INTERFERENCE,     ///< 0.3 ≤ h/w ≤ 0.7 - Turbulent wakes interact
    SKIMMING_FLOW          ///< h/w > 0.7 - Stable recirculation vortex (IDEAL)
};

/**
 * @brief Canyon geometric properties
 */
struct Canyon {
    std::string id;           ///< Unique canyon identifier
    double latitude;          ///< Geographic latitude (degrees)
    double longitude;         ///< Geographic longitude (degrees)
    double height_m;          ///< Building height (meters)
    double width_m;           ///< Street width (meters)
    double orientation_deg;   ///< Canyon axis orientation from North (degrees)
    
    /**
     * @brief Calculate aspect ratio h/w
     * @return Height-to-width ratio
     */
    double aspectRatio() const {
        return height_m / width_m;
    }
    
    /**
     * @brief Classify flow regime based on aspect ratio
     * @return FlowRegime enum value
     */
    FlowRegime classifyFlowRegime() const {
        double ratio = aspectRatio();
        if (ratio < 0.3) {
            return FlowRegime::ISOLATED_ROUGHNESS;
        } else if (ratio <= 0.7) {
            return FlowRegime::WAKE_INTERFERENCE;
        } else {
            return FlowRegime::SKIMMING_FLOW;
        }
    }
    
    /**
     * @brief Check if canyon is suitable for windnet deployment
     * @return true if skimming-flow regime (h/w ≥ 0.7)
     */
    bool isSuitableForWindnet() const {
        return classifyFlowRegime() == FlowRegime::SKIMMING_FLOW;
    }
};

/**
 * @brief Windnet geometric specifications
 */
struct Windnet {
    std::string canyon_id;    ///< Reference to parent canyon
    double height_m;          ///< Net vertical span (meters)
    double width_m;           ///< Net horizontal span (meters)
    double porosity;          ///< Open area fraction (0.0 - 1.0)
    double anchor_height_m;   ///< Bottom edge height above street (meters)
    
    /**
     * @brief Calculate net area
     * @return Area in square meters
     */
    double area() const {
        return height_m * width_m;
    }
    
    /**
     * @brief Check if net dimensions are within recommended bounds
     * @param canyon Reference canyon for validation
     * @return true if dimensions are valid
     */
    bool validateDimensions(const Canyon& canyon) const {
        // Net height should span z/h ∈ [0.3, 0.8] of canyon
        double min_height = 0.3 * canyon.height_m;
        double max_height = 0.8 * canyon.height_m;
        
        // Net should not exceed canyon width
        if (width_m > canyon.width_m * 0.8) {
            return false;
        }
        
        // Porosity should be in optimal range
        if (porosity < 0.5 || porosity > 0.7) {
            return false;
        }
        
        return true;
    }
};

/**
 * @brief Calculate optimal net vertical placement for a canyon
 * 
 * Based on vortex dynamics, the descending branch of the recirculation
 * vortex has peak velocities at z/h ≈ 0.3–0.7. Net should overlap this zone.
 * 
 * @param canyon Reference canyon
 * @return Windnet struct with optimal vertical placement
 */
inline Windnet calculateOptimalPlacement(const Canyon& canyon) {
    Windnet net;
    net.canyon_id = canyon.id;
    
    // Optimal vertical span: z/h ∈ [0.3, 0.8]
    double z_min = 0.3 * canyon.height_m;
    double z_max = 0.8 * canyon.height_m;
    
    net.anchor_height_m = z_min;
    net.height_m = z_max - z_min;
    
    // Horizontal span: use 60-80% of street width for corner placement
    net.width_m = canyon.width_m * 0.7;
    
    // Optimal porosity for macro-litter capture
    net.porosity = 0.6;
    
    return net;
}

/**
 * @brief Convert FlowRegime to string for reporting
 * @param regime Flow regime enum
 * @return Human-readable string
 */
inline std::string flowRegimeToString(FlowRegime regime) {
    switch (regime) {
        case FlowRegime::ISOLATED_ROUGHNESS:
            return "isolated_roughness";
        case FlowRegime::WAKE_INTERFERENCE:
            return "wake_interference";
        case FlowRegime::SKIMMING_FLOW:
            return "skimming_flow";
        default:
            return "unknown";
    }
}

/**
 * @brief Parse FlowRegime from string
 * @param str String representation
 * @return FlowRegime enum value
 */
inline FlowRegime stringToFlowRegime(const std::string& str) {
    if (str == "isolated_roughness") return FlowRegime::ISOLATED_ROUGHNESS;
    if (str == "wake_interference") return FlowRegime::WAKE_INTERFERENCE;
    if (str == "skimming_flow") return FlowRegime::SKIMMING_FLOW;
    return FlowRegime::ISOLATED_ROUGHNESS; // default
}

} // namespace geometry
} // namespace windminer

#endif // WINDNET_GEOMETRY_HPP
