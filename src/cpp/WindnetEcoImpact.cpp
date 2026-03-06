/**
 * @file WindnetEcoImpact.cpp
 * @brief Implementation of eco-impact scoring system
 */

#include "WindnetEcoImpact.hpp"
#include <algorithm>
#include <cmath>

namespace windminer {
namespace impact {

/**
 * @brief Calculate eco-impact score
 * Formula: E = (M_cap / M_ref) · (1 - R_nuisance)
 */
double EcoImpactCalculator::calculateScore(
    double capture_kg_year,
    const NuisanceFactors& nuisance
) const {
    // Normalize capture mass
    double benefit_ratio = capture_kg_year / reference_capture_kg_;
    
    // Cap benefit ratio at 1.0 for scoring normalization
    benefit_ratio = std::min(1.0, benefit_ratio);
    
    // Calculate nuisance penalty
    double penalty = nuisance.total();
    
    // Calculate final score
    double score = benefit_ratio * (1.0 - penalty);
    
    // Clamp to [0.0, 1.0]
    return std::max(0.0, std::min(1.0, score));
}

/**
 * @brief Evaluate full eco-impact for a canyon
 */
EcoImpactResult EcoImpactCalculator::evaluate(
    const model::CaptureResult& capture_result,
    const NuisanceFactors& nuisance
) const {
    EcoImpactResult result;
    result.canyon_id = capture_result.canyon_id;
    result.capture_mass_kg_year = capture_result.expected_capture_kg_year;
    result.reference_mass_kg_year = reference_capture_kg_;
    result.nuisance_score = nuisance.total();
    
    result.eco_impact_score = calculateScore(
        result.capture_mass_kg_year,
        nuisance
    );
    
    result.recommendation = scoreToRecommendation(result.eco_impact_score);
    result.karma_per_unit = calculateKarma(result);
    
    return result;
}

/**
 * @brief Calculate Karma units for ALN blockchain
 * Karma = EcoScore * CaptureMass * ScalingFactor
 */
double EcoImpactCalculator::calculateKarma(const EcoImpactResult& eco_result) const {
    // Scaling factor to keep Karma units manageable (e.g., 10 Karma per kg at score 1.0)
    const double scaling_factor = 10.0;
    return eco_result.eco_impact_score * eco_result.capture_mass_kg_year * scaling_factor;
}

} // namespace impact
} // namespace windminer
