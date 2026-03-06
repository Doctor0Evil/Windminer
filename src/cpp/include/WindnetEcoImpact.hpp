/**
 * @file WindnetEcoImpact.hpp
 * @brief Eco-impact scoring system for windnet deployments
 * 
 * Implements the eco-impact score formula:
 * E_windnet = (M_cap / M_ref) · (1 - R_nuisance)
 * 
 * Where R_nuisance includes penalties for:
 * - Visual complexity
 * - Pedestrian safety
 * - Maintenance burden
 * 
 * @author Ecotribute Research Team
 * @date 2026
 */

#ifndef WINDNET_ECO_IMPACT_HPP
#define WINDNET_ECO_IMPACT_HPP

#include "WindnetCaptureModel.hpp"
#include <string>

namespace windminer {
namespace impact {

/**
 * @brief Nuisance factor components
 */
struct NuisanceFactors {
    double visual_penalty;      ///< 0.0-0.1 (visual clutter)
    double safety_penalty;      ///< 0.0-0.1 (pedestrian/traffic safety)
    double maintenance_penalty; ///< 0.0-0.1 (cleaning frequency burden)
    
    /**
     * @brief Calculate total nuisance score
     * @return Sum of all penalties (capped at 0.3)
     */
    double total() const {
        double sum = visual_penalty + safety_penalty + maintenance_penalty;
        return (sum > 0.3) ? 0.3 : sum;
    }
    
    /**
     * @brief Default constructor (minimal nuisance)
     */
    NuisanceFactors()
        : visual_penalty(0.02)
        , safety_penalty(0.02)
        , maintenance_penalty(0.03)
    {}
};

/**
 * @brief Eco-impact score result
 */
struct EcoImpactResult {
    std::string canyon_id;
    double capture_mass_kg_year;     ///< M_cap
    double reference_mass_kg_year;   ///< M_ref
    double nuisance_score;           ///< R_nuisance
    double eco_impact_score;         ///< E_windnet (0.0-1.0)
    std::string recommendation;      ///< DEPLOY, REVIEW, SKIP
    double karma_per_unit;           ///< Karma units for ALN tracking
    
    /**
     * @brief Check if deployment is recommended
     * @return true if eco_impact_score ≥ 0.80
     */
    bool isRecommended() const {
        return eco_impact_score >= 0.80;
    }
};

/**
 * @brief Eco-impact calculator
 */
class EcoImpactCalculator {
private:
    double reference_capture_kg_;   ///< M_ref for normalization
    
public:
    /**
     * @brief Default constructor
     * @param reference_capture_kg Reference mass (default: 150 kg/year)
     */
    explicit EcoImpactCalculator(double reference_capture_kg = 150.0)
        : reference_capture_kg_(reference_capture_kg)
    {}
    
    /**
     * @brief Calculate eco-impact score
     * 
     * Formula: E = (M_cap / M_ref) · (1 - R_nuisance)
     * 
     * @param capture_kg_year Expected annual capture (kg)
     * @param nuisance Nuisance factors
     * @return Eco-impact score (0.0-1.0)
     */
    double calculateScore(
        double capture_kg_year,
        const NuisanceFactors& nuisance
    ) const;
    
    /**
     * @brief Evaluate full eco-impact for a canyon
     * @param capture_result Capture model result
     * @param nuisance Nuisance factors
     * @return EcoImpactResult struct
     */
    EcoImpactResult evaluate(
        const model::CaptureResult& capture_result,
        const NuisanceFactors& nuisance
    ) const;
    
    /**
     * @brief Calculate Karma units for ALN blockchain
     * 
     * Karma is awarded based on verified capture mass and eco-score
     * 
     * @param eco_result Eco-impact result
     * @return Karma units per year
     */
    double calculateKarma(const EcoImpactResult& eco_result) const;
    
    /**
     * @brief Get reference capture mass
     * @return M_ref (kg/year)
     */
    double getReferenceCapture() const {
        return reference_capture_kg_;
    }
    
    /**
     * @brief Set reference capture mass
     * @param kg Reference mass (kg/year)
     */
    void setReferenceCapture(double kg) {
        reference_capture_kg_ = kg;
    }
};

/**
 * @brief Recommendation threshold constants
 */
namespace thresholds {
    constexpr double EXCELLENT_SCORE = 0.90;   ///< Exceptional eco-impact
    constexpr double GOOD_SCORE = 0.80;        ///< Recommended for deployment
    constexpr double REVIEW_SCORE = 0.60;      ///< Needs further analysis
    constexpr double SKIP_SCORE = 0.60;        ///< Below threshold
}

/**
 * @brief Convert eco-impact score to recommendation string
 * @param score Eco-impact score (0.0-1.0)
 * @return Recommendation string
 */
inline std::string scoreToRecommendation(double score) {
    if (score >= thresholds::EXCELLENT_SCORE) {
        return "DEPLOY_PRIORITY";
    } else if (score >= thresholds::GOOD_SCORE) {
        return "DEPLOY";
    } else if (score >= thresholds::REVIEW_SCORE) {
        return "REVIEW";
    } else {
        return "SKIP";
    }
}

} // namespace impact
} // namespace windminer

#endif // WINDNET_ECO_IMPACT_HPP
