//! Sensor firmware utilities for wind and litter detection

use serde::{Deserialize, Serialize};
use crate::{Result, Error};

/// Wind sensor reading
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct WindReading {
    pub timestamp_ms: u64,
    pub speed_ms: f64,
    pub direction_deg: f64,
    pub temperature_c: f64,
    pub humidity_percent: f64,
}

/// Litter impact event from optical/acoustic sensor
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct LitterImpact {
    pub timestamp_ms: u64,
    pub impact_count: u32,
    pub estimated_mass_g: f64,
    pub confidence: f64,
}

/// Aggregated sensor data for transmission
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SensorReport {
    pub node_id: String,
    pub report_interval_ms: u64,
    pub wind_readings: Vec<WindReading>,
    pub litter_impacts: Vec<LitterImpact>,
    pub battery_percent: f64,
    pub signal_strength_dbm: i16,
}

impl SensorReport {
    /// Create new sensor report
    pub fn new(node_id: String, report_interval_ms: u64) -> Self {
        SensorReport {
            node_id,
            report_interval_ms,
            wind_readings: Vec::new(),
            litter_impacts: Vec::new(),
            battery_percent: 100.0,
            signal_strength_dbm: -50,
        }
    }

    /// Calculate average wind speed from readings
    pub fn average_wind_speed(&self) -> Option<f64> {
        if self.wind_readings.is_empty() {
            return None;
        }
        let sum: f64 = self.wind_readings.iter().map(|r| r.speed_ms).sum();
        Some(sum / self.wind_readings.len() as f64)
    }

    /// Calculate total litter mass from impacts
    pub fn total_litter_mass_g(&self) -> f64 {
        self.litter_impacts.iter().map(|i| i.estimated_mass_g).sum()
    }

    /// Check if report is valid for transmission
    pub fn is_valid(&self) -> bool {
        self.battery_percent > 10.0 && !self.wind_readings.is_empty()
    }
}

/// Sensor configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SensorConfig {
    pub node_id: String,
    pub sample_interval_ms: u64,
    pub report_interval_ms: u64,
    pub wind_threshold_ms: f64,
    pub server_url: String,
}

impl Default for SensorConfig {
    fn default() -> Self {
        SensorConfig {
            node_id: "SENSOR-001".to_string(),
            sample_interval_ms: 1000,
            report_interval_ms: 3600000, // 1 hour
            wind_threshold_ms: 1.5,
            server_url: "http://localhost:8080/api/sensor".to_string(),
        }
    }
}

/// Simulated sensor data generator for testing
pub fn generate_simulated_reading(timestamp_ms: u64) -> WindReading {
    use std::time::{SystemTime, UNIX_EPOCH};
    
    // Simple simulation with some variation
    let base_speed = 3.0;
    let variation = ((timestamp_ms as f64 * 0.001).sin() * 0.5) + 1.0;
    
    WindReading {
        timestamp_ms,
        speed_ms: base_speed * variation,
        direction_deg: 270.0 + (timestamp_ms as f64 * 0.0001).sin() * 30.0,
        temperature_c: 25.0 + (timestamp_ms as f64 * 0.00001).sin() * 5.0,
        humidity_percent: 30.0,
    }
}

/// Validate sensor reading ranges
pub fn validate_reading(reading: &WindReading) -> Result<()> {
    if reading.speed_ms < 0.0 || reading.speed_ms > 50.0 {
        return Err(Error::Sensor(
            format!("Invalid wind speed: {}", reading.speed_ms)
        ));
    }
    if reading.direction_deg < 0.0 || reading.direction_deg > 360.0 {
        return Err(Error::Sensor(
            format!("Invalid wind direction: {}", reading.direction_deg)
        ));
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sensor_report_aggregation() {
        let mut report = SensorReport::new("TEST-001".to_string(), 3600000);
        
        for i in 0..10 {
            report.wind_readings.push(generate_simulated_reading(i * 1000));
        }

        let avg_speed = report.average_wind_speed().unwrap();
        assert!(avg_speed > 2.0 && avg_speed < 4.0);
    }

    #[test]
    fn test_litter_mass_calculation() {
        let mut report = SensorReport::new("TEST-001".to_string(), 3600000);
        
        report.litter_impacts.push(LitterImpact {
            timestamp_ms: 1000,
            impact_count: 5,
            estimated_mass_g: 10.0,
            confidence: 0.9,
        });

        assert_eq!(report.total_litter_mass_g(), 10.0);
    }
}
