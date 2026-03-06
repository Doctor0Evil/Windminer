//! Physics-based litter capture model implementation

use serde::{Deserialize, Serialize};
use crate::geometry::{Canyon, Windnet};
use crate::{Result, Error};

/// Capture model parameters (Phoenix-specific defaults)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CaptureParameters {
    /// Litter concentration Cℓ* (kg/m³)
    pub litter_concentration: f64,
    /// Impingement speed Uimp* (m/s)
    pub impingement_speed: f64,
    /// Net efficiency ηnet (0.0-1.0)
    pub net_efficiency: f64,
    /// Effective wind time Twind (seconds)
    pub effective_wind_seconds: f64,
    /// Reference capture mass Mref (kg/year)
    pub reference_capture_kg: f64,
}

impl Default for CaptureParameters {
    fn default() -> Self {
        CaptureParameters {
            litter_concentration: 1.0e-8,      // 10 µg/m³
            impingement_speed: 2.5,            // m/s
            net_efficiency: 0.4,               // 40%
            effective_wind_seconds: 7.2e6,     // 2000 hours
            reference_capture_kg: 150.0,       // kg/year
        }
    }
}

impl CaptureParameters {
    /// Validate parameter ranges
    pub fn validate(&self) -> Result<()> {
        if self.litter_concentration <= 0.0 {
            return Err(Error::InvalidParameter(
                "Litter concentration must be positive".to_string()
            ));
        }
        if self.impingement_speed <= 0.0 {
            return Err(Error::InvalidParameter(
                "Impingement speed must be positive".to_string()
            ));
        }
        if self.net_efficiency < 0.0 || self.net_efficiency > 1.0 {
            return Err(Error::InvalidParameter(
                "Net efficiency must be in [0.0, 1.0]".to_string()
            ));
        }
        Ok(())
    }
}

/// Capture model result
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CaptureResult {
    pub canyon_id: String,
    pub expected_capture_kg_year: f64,
    pub net_area_m2: f64,
    pub effective_wind_hours: f64,
    pub is_suitable: bool,
    pub flow_regime: String,
}

/// Windnet capture model calculator
#[derive(Debug, Clone)]
pub struct CaptureModel {
    params: CaptureParameters,
}

impl Default for CaptureModel {
    fn default() -> Self {
        CaptureModel::new(CaptureParameters::default())
    }
}

impl CaptureModel {
    /// Create new capture model with parameters
    pub fn new(params: CaptureParameters) -> Self {
        CaptureModel { params }
    }

    /// Calculate expected annual litter capture
    /// Formula: M_cap = Cℓ* · Uimp* · Anet · ηnet · Twind
    pub fn calculate_capture(&self, canyon: &Canyon, net: &Windnet) -> f64 {
        if !canyon.is_suitable() {
            return 0.0;
        }

        self.params.litter_concentration
            * self.params.impingement_speed
            * net.area()
            * self.params.net_efficiency
            * self.params.effective_wind_seconds
    }

    /// Evaluate full capture result for a canyon
    pub fn evaluate_canyon(&self, canyon: &Canyon) -> CaptureResult {
        let is_suitable = canyon.is_suitable();
        let flow_regime = canyon.classify_flow_regime().as_str().to_string();

        if !is_suitable {
            return CaptureResult {
                canyon_id: canyon.id.clone(),
                expected_capture_kg_year: 0.0,
                net_area_m2: 0.0,
                effective_wind_hours: 0.0,
                is_suitable: false,
                flow_regime,
            };
        }

        let net = Windnet::optimal_for_canyon(canyon);
        let capture = self.calculate_capture(canyon, &net);

        CaptureResult {
            canyon_id: canyon.id.clone(),
            expected_capture_kg_year: capture,
            net_area_m2: net.area(),
            effective_wind_hours: self.params.effective_wind_seconds / 3600.0,
            is_suitable: true,
            flow_regime,
        }
    }

    /// Get parameters
    pub fn parameters(&self) -> &CaptureParameters {
        &self.params
    }

    /// Set parameters
    pub fn set_parameters(&mut self, params: CaptureParameters) {
        self.params = params;
    }
}

/// Sensitivity analysis result
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SensitivityResult {
    pub parameter_name: String,
    pub baseline_value: f64,
    pub perturbed_value: f64,
    pub baseline_capture: f64,
    pub perturbed_capture: f64,
    pub sensitivity_coefficient: f64,
}

/// Perform sensitivity analysis on capture model
pub fn perform_sensitivity_analysis(
    canyon: &Canyon,
    net: &Windnet,
    params: &CaptureParameters,
) -> Vec<SensitivityResult> {
    let model = CaptureModel::new(params.clone());
    let baseline_capture = model.calculate_capture(canyon, net);

    let mut results = Vec::new();

    // Test each parameter with +10% perturbation
    let perturbations = [
        ("litter_concentration", params.litter_concentration),
        ("impingement_speed", params.impingement_speed),
        ("net_efficiency", params.net_efficiency),
    ];

    for (name, base_val) in perturbations.iter() {
        let mut perturbed_params = params.clone();
        
        match *name {
            "litter_concentration" => perturbed_params.litter_concentration = base_val * 1.1,
            "impingement_speed" => perturbed_params.impingement_speed = base_val * 1.1,
            "net_efficiency" => perturbed_params.net_efficiency = base_val * 1.1,
            _ => continue,
        }

        let perturbed_model = CaptureModel::new(perturbed_params);
        let perturbed_capture = perturbed_model.calculate_capture(canyon, net);

        let pct_change_in = 0.1; // 10%
        let pct_change_out = (perturbed_capture - baseline_capture) / baseline_capture;
        let sensitivity = pct_change_out / pct_change_in;

        results.push(SensitivityResult {
            parameter_name: name.to_string(),
            baseline_value: *base_val,
            perturbed_value: base_val * 1.1,
            baseline_capture,
            perturbed_capture,
            sensitivity_coefficient: sensitivity,
        });
    }

    results
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::geometry::Canyon;

    #[test]
    fn test_capture_calculation() {
        let canyon = Canyon::new(
            "test".to_string(),
            33.4484,
            -112.0740,
            30.0,
            20.0,
            0.0,
        ).unwrap();

        let net = Windnet::optimal_for_canyon(&canyon);
        let model = CaptureModel::default();
        let capture = model.calculate_capture(&canyon, &net);

        // Expected ~144 kg/year with default parameters
        assert!(capture > 100.0 && capture < 200.0);
    }

    #[test]
    fn test_unsuitable_canyon() {
        let canyon = Canyon::new(
            "test".to_string(),
            33.4484,
            -112.0740,
            5.0,   // Low building
            20.0,  // Wide street
            0.0,
        ).unwrap();

        let model = CaptureModel::default();
        let result = model.evaluate_canyon(&canyon);

        assert!(!result.is_suitable);
        assert_eq!(result.expected_capture_kg_year, 0.0);
    }
}
