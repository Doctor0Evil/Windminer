/**
 * @file WindClimatePhoenix.hpp
 * @brief Phoenix-specific wind climate data and cross-canyon wind calculations
 * 
 * This module provides:
 * - Phoenix wind rose statistics (seasonal direction distributions)
 * - Cross-canyon wind component calculation: U⊥ = U · |cos(θw - θc)|
 * - Effective wind time calculation (hours with U⊥ ≥ Umin)
 * 
 * @author Ecotribute Research Team
 * @date 2026
 */

#ifndef WIND_CLIMATE_PHOENIX_HPP
#define WIND_CLIMATE_PHOENIX_HPP

#include <string>
#include <vector>
#include <cmath>
#include <map>

namespace windminer {
namespace climate {

/**
 * @brief Wind direction sector enumeration
 */
enum class WindSector {
    N, NE, E, SE, S, SW, W, NW
};

/**
 * @brief Seasonal wind pattern for Phoenix
 * 
 * Phoenix exhibits bimodal wind patterns:
 * - West-dominant: March–September (~6.7 months, ~54% of hours)
 * - East-dominant: September–March (~5.4 months, ~53% of hours)
 */
struct SeasonalWindPattern {
    std::string season_name;      ///< "west_dominant" or "east_dominant"
    int start_month;              ///< Starting month (1-12)
    int end_month;                ///< Ending month (1-12)
    double dominant_direction_deg; ///< Prevailing wind direction (degrees from North)
    double frequency;             ///< Fraction of hours in this pattern (0.0-1.0)
};

/**
 * @brief Hourly wind record structure
 */
struct HourlyWindRecord {
    int year;
    int month;
    int day;
    int hour;
    double speed_ms;              ///< Wind speed at 10m (m/s)
    double direction_deg;         ///< Wind direction from North (degrees)
};

/**
 * @brief Phoenix wind climate statistics
 */
class PhoenixWindClimate {
private:
    std::vector<HourlyWindRecord> hourly_data_;
    double mean_annual_speed_ms_;
    double effective_wind_hours_;
    
    // Seasonal patterns (Phoenix-specific)
    std::vector<SeasonalWindPattern> seasonal_patterns_;
    
public:
    /**
     * @brief Default constructor with Phoenix baseline parameters
     */
    PhoenixWindClimate() 
        : mean_annual_speed_ms_(3.15)  // Phoenix average: 3.0-3.3 m/s
        , effective_wind_hours_(2000)  // Hours with U⊥ ≥ 1.5 m/s
    {
        // Initialize Phoenix seasonal patterns
        seasonal_patterns_.push_back({
            "west_dominant", 3, 9, 270.0, 0.54  // W = 270°
        });
        seasonal_patterns_.push_back({
            "east_dominant", 10, 2, 90.0, 0.53  // E = 90°
        });
    }
    
    /**
     * @brief Load wind data from CSV file
     * @param filepath Path to wind data CSV
     * @return true if loaded successfully
     */
    bool loadFromCSV(const std::string& filepath);
    
    /**
     * @brief Calculate cross-canyon wind component
     * 
     * Formula: U⊥ = U · |cos(θw - θc)|
     * 
     * @param wind_speed Wind speed (m/s)
     * @param wind_direction_deg Wind direction (degrees from North)
     * @param canyon_orientation_deg Canyon axis orientation (degrees from North)
     * @return Cross-canyon wind component (m/s)
     */
    static double calculateCrossCanyonWind(
        double wind_speed,
        double wind_direction_deg,
        double canyon_orientation_deg
    ) {
        double wind_rad = wind_direction_deg * M_PI / 180.0;
        double canyon_rad = canyon_orientation_deg * M_PI / 180.0;
        return wind_speed * std::abs(std::cos(wind_rad - canyon_rad));
    }
    
    /**
     * @brief Calculate effective wind time for a canyon orientation
     * 
     * Counts hours where U⊥ ≥ Umin threshold
     * 
     * @param canyon_orientation_deg Canyon axis orientation (degrees)
     * @param threshold_ms Minimum wind speed threshold (default: 1.5 m/s)
     * @return Effective wind hours per year
     */
    double calculateEffectiveWindTime(
        double canyon_orientation_deg,
        double threshold_ms = 1.5
    ) const;
    
    /**
     * @brief Get mean annual wind speed
     * @return Wind speed (m/s)
     */
    double getMeanAnnualSpeed() const {
        return mean_annual_speed_ms_;
    }
    
    /**
     * @brief Get effective wind hours baseline
     * @return Hours per year
     */
    double getEffectiveWindHours() const {
        return effective_wind_hours_;
    }
    
    /**
     * @brief Get seasonal patterns
     * @return Vector of SeasonalWindPattern
     */
    const std::vector<SeasonalWindPattern>& getSeasonalPatterns() const {
        return seasonal_patterns_;
    }
    
    /**
     * @brief Determine dominant season for a canyon orientation
     * @param canyon_orientation_deg Canyon orientation (degrees)
     * @return Season name string
     */
    std::string getDominantSeason(double canyon_orientation_deg) const;
};

/**
 * @brief Wind rose data structure for CSV export
 */
struct WindRoseData {
    std::string sector;           ///< Direction sector (N, NE, E, etc.)
    double frequency;             ///< Fraction of hours from this direction
    double mean_speed_ms;         ///< Mean wind speed for this sector
};

} // namespace climate
} // namespace windminer

#endif // WIND_CLIMATE_PHOENIX_HPP
