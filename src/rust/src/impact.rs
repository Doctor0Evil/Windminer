//! Eco-impact scoring system implementation

use serde::{Deserialize, Serialize};
use crate::capture::CaptureResult;
use crate::thresholds;

/// Nuisance factor components
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NuisanceFactors {
    /// Visual clutter penalty (0.0-0.1)
    pub visual_penalty: f64,
    /// Pedestrian/traffic safety penalty (0.0-0.1)
    pub safety_penalty: f64,
    /// Maintenance burden penalty (0.0-0.1)
    pub maintenance_penalty: f64,
}

impl Default for NuisanceFactors {
    fn default() -> Self {
        NuisanceFactors {
            visual_penalty: 0.02,
            safety_penalty: 0.02,
            maintenance_penalty: 0.03,
        }
    }
}

impl NuisanceFactors {
    /// Calculate total nuisance score (capped at 0.3)
    pub fn total(&self) -> f64 {
        let sum = self.visual_penalty + self.safety_penalty + self.maintenance_penalty;
        sum.min(0.3)
    }

    /// Create high-nuisance factors for testing
    pub fn high_nuisance() -> Self {
        NuisanceFactors {
            visual_penalty: 0.1,
            safety_penalty: 0.1,
            maintenance_penalty: 0.1,
        }
    }
}

/// Eco-impact score result
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct EcoImpactResult {
    pub canyon_id: String,
    pub capture_mass_kg_year: f64,
    pub reference_mass_kg_year: f64,
    pub nuisance_score: f64,
    pub eco_impact_score: f64,
    pub recommendation: String,
    pub karma_per_unit: f64,
}

impl EcoImpactResult {
    /// Check if deployment is recommended
    pub fn is_recommended(&self) -> bool {
        self.eco_impact_score >= thresholds::GOOD
    }
}

/// Eco-impact calculator
#[derive(Debug, Clone)]
pub struct EcoImpactCalculator {
    reference_capture_kg: f64,
}

impl Default for EcoImpactCalculator {
    fn default() -> Self {
        EcoImpactCalculator::new(150.0)
    }
}

impl EcoImpactCalculator {
    /// Create new calculator with reference mass
    pub fn new(reference_capture_kg: f64) -> Self {
        EcoImpactCalculator { reference_capture_kg }
    }

    /// Calculate eco-impact score
    /// Formula: E = (M_cap / M_ref) · (1 - R_nuisance)
    pub fn calculate_score(&self, capture_kg_year: f64, nuisance: &NuisanceFactors) -> f64 {
        let benefit_ratio = (capture_kg_year / self.reference_capture_kg).min(1.0);
        let penalty = nuisance.total();
        let score = benefit_ratio * (1.0 - penalty);
        score.max(0.0).min(1.0)
    }

    /// Evaluate full eco-impact for a canyon
    pub fn evaluate(&self, capture_result: &CaptureResult, nuisance: &NuisanceFactors) -> EcoImpactResult {
        let eco_score = self.calculate_score(capture_result.expected_capture_kg_year, nuisance);
        let recommendation = score_to_recommendation(eco_score);
        let karma = calculate_karma(eco_score, capture_result.expected_capture_kg_year);

        EcoImpactResult {
            canyon_id: capture_result.canyon_id.clone(),
            capture_mass_kg_year: capture_result.expected_capture_kg_year,
            reference_mass_kg_year: self.reference_capture_kg,
            nuisance_score: nuisance.total(),
            eco_impact_score: eco_score,
            recommendation,
            karma_per_unit: karma,
        }
    }

    /// Get reference capture mass
    pub fn reference_capture(&self) -> f64 {
        self.reference_capture_kg
    }
}

/// Convert eco-impact score to recommendation string
pub fn score_to_recommendation(score: f64) -> String {
    if score >= thresholds::EXCELLENT {
        "DEPLOY_PRIORITY".to_string()
    } else if score >= thresholds::GOOD {
        "DEPLOY".to_string()
    } else if score >= thresholds::REVIEW {
        "REVIEW".to_string()
    } else {
        "SKIP".to_string()
    }
}

/// Calculate Karma units for ALN blockchain
/// Karma = EcoScore × CaptureMass × ScalingFactor
pub fn calculate_karma(eco_score: f64, capture_kg_year: f64) -> f64 {
    const SCALING_FACTOR: f64 = 10.0;
    eco_score * capture_kg_year * SCALING_FACTOR
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::capture::CaptureResult;

    #[test]
    fn test_eco_score_calculation() {
        let calc = EcoImpactCalculator::default();
        let nuisance = NuisanceFactors::default();

        // 150 kg capture with low nuisance should score ~0.93
        let score = calc.calculate_score(150.0, &nuisance);
        assert!(score > 0.9);
    }

    #[test]
    fn test_high_nuisance_penalty() {
        let calc = EcoImpactCalculator::default();
        let nuisance = NuisanceFactors::high_nuisance();

        // Same capture with high nuisance should score lower
        let score = calc.calculate_score(150.0, &nuisance);
        assert!(score < 0.75);
    }

    #[test]
    fn test_recommendation_thresholds() {
        assert_eq!(score_to_recommendation(0.95), "DEPLOY_PRIORITY");
        assert_eq!(score_to_recommendation(0.85), "DEPLOY");
        assert_eq!(score_to_recommendation(0.70), "REVIEW");
        assert_eq!(score_to_recommendation(0.50), "SKIP");
    }
}
