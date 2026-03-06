//! # Windminer Rust Library
//! 
//! Safe data processing, sensor integration, and FFI bindings for C++ core.
//! 
//! ## Modules
//! 
//! - `geometry`: Canyon and windnet geometric calculations
//! - `climate`: Wind climate data processing
//! - `capture`: Litter capture model implementation
//! - `impact`: Eco-impact scoring
//! - `sensor`: Sensor firmware utilities
//! - `ffi`: C++ interoperability layer

pub mod geometry;
pub mod climate;
pub mod capture;
pub mod impact;
pub mod sensor;
pub mod ffi;

// Re-export main types for convenience
pub use geometry::{Canyon, Windnet, FlowRegime};
pub use capture::{CaptureModel, CaptureParameters, CaptureResult};
pub use impact::{EcoImpactCalculator, EcoImpactResult, NuisanceFactors};

/// Library version
pub const VERSION: &str = env!("CARGO_PKG_VERSION");

/// Default reference capture mass (kg/year)
pub const DEFAULT_REFERENCE_CAPTURE_KG: f64 = 150.0;

/// Minimum wind speed threshold for litter mobilization (m/s)
pub const WIND_THRESHOLD_MS: f64 = 1.5;

/// Optimal net porosity range
pub const NET_POROSITY_MIN: f64 = 0.5;
pub const NET_POROSITY_MAX: f64 = 0.7;

/// Eco-impact score thresholds
pub mod thresholds {
    pub const EXCELLENT: f64 = 0.90;
    pub const GOOD: f64 = 0.80;
    pub const REVIEW: f64 = 0.60;
}

/// Result type alias for windminer operations
pub type Result<T> = std::result::Result<T, Error>;

/// Error types for windminer operations
#[derive(Debug, thiserror::Error)]
pub enum Error {
    #[error("IO error: {0}")]
    Io(#[from] std::io::Error),
    
    #[error("CSV parsing error: {0}")]
    Csv(#[from] csv::Error),
    
    #[error("Invalid geometry: {0}")]
    InvalidGeometry(String),
    
    #[error("Invalid parameter: {0}")]
    InvalidParameter(String),
    
    #[error("Sensor error: {0}")]
    Sensor(String),
    
    #[error("FFI error: {0}")]
    Ffi(String),
}

/// Initialize logging for the library
pub fn init_logging() {
    env_logger::Builder::from_env(
        env_logger::Env::default().default_filter_or("info")
    ).init();
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_version() {
        assert_eq!(VERSION, "1.0.0");
    }

    #[test]
    fn test_thresholds() {
        assert!(thresholds::EXCELLENT > thresholds::GOOD);
        assert!(thresholds::GOOD > thresholds::REVIEW);
    }
}
