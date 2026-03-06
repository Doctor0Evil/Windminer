//! Canyon geometry classification and windnet placement calculations

use serde::{Deserialize, Serialize};
use crate::{Error, Result};

/// Flow regime classification based on canyon aspect ratio
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum FlowRegime {
    #[serde(rename = "isolated_roughness")]
    IsolatedRoughness,
    #[serde(rename = "wake_interference")]
    WakeInterference,
    #[serde(rename = "skimming_flow")]
    SkimmingFlow,
}

impl FlowRegime {
    /// Classify flow regime from aspect ratio (h/w)
    pub fn from_aspect_ratio(ratio: f64) -> Self {
        if ratio < 0.3 {
            FlowRegime::IsolatedRoughness
        } else if ratio <= 0.7 {
            FlowRegime::WakeInterference
        } else {
            FlowRegime::SkimmingFlow
        }
    }

    /// Check if suitable for windnet deployment
    pub fn is_suitable(&self) -> bool {
        matches!(self, FlowRegime::SkimmingFlow)
    }

    /// Convert to string representation
    pub fn as_str(&self) -> &'static str {
        match self {
            FlowRegime::IsolatedRoughness => "isolated_roughness",
            FlowRegime::WakeInterference => "wake_interference",
            FlowRegime::SkimmingFlow => "skimming_flow",
        }
    }
}

/// Canyon geometric properties
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Canyon {
    pub id: String,
    pub latitude: f64,
    pub longitude: f64,
    pub height_m: f64,
    pub width_m: f64,
    pub orientation_deg: f64,
}

impl Canyon {
    /// Create a new canyon instance
    pub fn new(
        id: String,
        latitude: f64,
        longitude: f64,
        height_m: f64,
        width_m: f64,
        orientation_deg: f64,
    ) -> Result<Self> {
        if height_m <= 0.0 || width_m <= 0.0 {
            return Err(Error::InvalidGeometry(
                "Building height and street width must be positive".to_string()
            ));
        }

        Ok(Canyon {
            id,
            latitude,
            longitude,
            height_m,
            width_m,
            orientation_deg,
        })
    }

    /// Calculate aspect ratio (h/w)
    pub fn aspect_ratio(&self) -> f64 {
        self.height_m / self.width_m
    }

    /// Classify flow regime
    pub fn classify_flow_regime(&self) -> FlowRegime {
        FlowRegime::from_aspect_ratio(self.aspect_ratio())
    }

    /// Check if suitable for windnet
    pub fn is_suitable(&self) -> bool {
        self.classify_flow_regime().is_suitable()
    }

    /// Calculate optimal net height range (z/h ∈ [0.3, 0.8])
    pub fn optimal_net_height_range(&self) -> (f64, f64) {
        (0.3 * self.height_m, 0.8 * self.height_m)
    }
}

/// Windnet geometric specifications
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Windnet {
    pub canyon_id: String,
    pub height_m: f64,
    pub width_m: f64,
    pub porosity: f64,
    pub anchor_height_m: f64,
}

impl Windnet {
    /// Create optimal windnet for a canyon
    pub fn optimal_for_canyon(canyon: &Canyon) -> Self {
        let (z_min, z_max) = canyon.optimal_net_height_range();
        
        Windnet {
            canyon_id: canyon.id.clone(),
            height_m: z_max - z_min,
            width_m: canyon.width_m * 0.7,
            porosity: 0.6,
            anchor_height_m: z_min,
        }
    }

    /// Calculate net area
    pub fn area(&self) -> f64 {
        self.height_m * self.width_m
    }

    /// Validate net dimensions
    pub fn validate(&self, canyon: &Canyon) -> Result<()> {
        // Check porosity range
        if self.porosity < 0.5 || self.porosity > 0.7 {
            return Err(Error::InvalidParameter(
                format!("Porosity {} outside valid range [0.5, 0.7]", self.porosity)
            ));
        }

        // Check net doesn't exceed canyon width
        if self.width_m > canyon.width_m * 0.8 {
            return Err(Error::InvalidGeometry(
                "Net width exceeds 80% of street width".to_string()
            ));
        }

        // Check safety clearance (minimum 4.5m for emergency vehicles)
        if self.anchor_height_m < 4.5 {
            return Err(Error::InvalidGeometry(
                "Anchor height below safety clearance (4.5m)".to_string()
            ));
        }

        Ok(())
    }
}

/// Calculate cross-canyon wind component
/// Formula: U⊥ = U · |cos(θw - θc)|
pub fn calculate_cross_canyon_wind(
    wind_speed: f64,
    wind_direction_deg: f64,
    canyon_orientation_deg: f64,
) -> f64 {
    let wind_rad = wind_direction_deg.to_radians();
    let canyon_rad = canyon_orientation_deg.to_radians();
    wind_speed * (wind_rad - canyon_rad).cos().abs()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_flow_regime_classification() {
        assert_eq!(FlowRegime::from_aspect_ratio(0.2), FlowRegime::IsolatedRoughness);
        assert_eq!(FlowRegime::from_aspect_ratio(0.5), FlowRegime::WakeInterference);
        assert_eq!(FlowRegime::from_aspect_ratio(1.5), FlowRegime::SkimmingFlow);
    }

    #[test]
    fn test_canyon_aspect_ratio() {
        let canyon = Canyon::new(
            "test".to_string(),
            33.4484,
            -112.0740,
            30.0,
            20.0,
            0.0,
        ).unwrap();
        assert!((canyon.aspect_ratio() - 1.5).abs() < 0.001);
    }

    #[test]
    fn test_cross_canyon_wind() {
        // Wind perpendicular to canyon should give full speed
        let u_perp = calculate_cross_canyon_wind(3.0, 90.0, 0.0);
        assert!(u_perp < 0.1); // Near zero when parallel

        let u_perp = calculate_cross_canyon_wind(3.0, 0.0, 0.0);
        assert!((u_perp - 3.0).abs() < 0.1); // Full speed when perpendicular
    }
}
