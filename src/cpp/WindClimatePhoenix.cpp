/**
 * @file WindClimatePhoenix.cpp
 * @brief Implementation of Phoenix wind climate data handling
 */

#include "WindClimatePhoenix.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

namespace windminer {
namespace climate {

/**
 * @brief Load wind data from CSV file
 * Expected format: year,month,day,hour,speed_ms,direction_deg
 */
bool PhoenixWindClimate::loadFromCSV(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open wind data file: " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    // Skip header
    std::getline(file, line);
    
    hourly_data_.clear();
    double total_speed = 0.0;
    int count = 0;
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        HourlyWindRecord record;
        
        // Parse CSV fields
        std::getline(ss, token, ','); record.year = std::stoi(token);
        std::getline(ss, token, ','); record.month = std::stoi(token);
        std::getline(ss, token, ','); record.day = std::stoi(token);
        std::getline(ss, token, ','); record.hour = std::stoi(token);
        std::getline(ss, token, ','); record.speed_ms = std::stod(token);
        std::getline(ss, token, ','); record.direction_deg = std::stod(token);
        
        hourly_data_.push_back(record);
        total_speed += record.speed_ms;
        count++;
    }
    
    if (count > 0) {
        mean_annual_speed_ms_ = total_speed / count;
    }
    
    return true;
}

/**
 * @brief Calculate effective wind time for a canyon orientation
 * Counts hours where cross-canyon wind U⊥ ≥ threshold
 */
double PhoenixWindClimate::calculateEffectiveWindTime(
    double canyon_orientation_deg,
    double threshold_ms
) const {
    if (hourly_data_.empty()) {
        // Return baseline estimate if no data loaded (Phoenix average)
        return effective_wind_hours_;
    }
    
    int effective_hours = 0;
    
    for (const auto& record : hourly_data_) {
        double u_perp = calculateCrossCanyonWind(
            record.speed_ms,
            record.direction_deg,
            canyon_orientation_deg
        );
        
        if (u_perp >= threshold_ms) {
            effective_hours++;
        }
    }
    
    return static_cast<double>(effective_hours);
}

/**
 * @brief Determine dominant season for a canyon orientation
 * Compares effective wind time across seasonal patterns
 */
std::string PhoenixWindClimate::getDominantSeason(double canyon_orientation_deg) const {
    std::string dominant = "west_dominant";
    double max_hours = 0.0;
    
    for (const auto& pattern : seasonal_patterns_) {
        // Simplified logic: check alignment with dominant direction
        double alignment = std::abs(std::cos(
            (pattern.dominant_direction_deg - canyon_orientation_deg) * M_PI / 180.0
        ));
        
        double weighted_hours = alignment * pattern.frequency * 8760.0; // 8760 hours/year
        
        if (weighted_hours > max_hours) {
            max_hours = weighted_hours;
            dominant = pattern.season_name;
        }
    }
    
    return dominant;
}

} // namespace climate
} // namespace windminer
