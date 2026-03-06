//! Sensor firmware main entry point
//! 
//! This binary runs on embedded sensor hardware to collect
//! wind and litter impact data, then transmits to central server.

use windminer_rs::sensor::*;
use std::thread;
use std::time::{Duration, SystemTime, UNIX_EPOCH};

fn main() -> anyhow::Result<()> {
    println!("Windminer Sensor Firmware v{}", env!("CARGO_PKG_VERSION"));
    
    let config = SensorConfig::default();
    println!("Node ID: {}", config.node_id);
    println!("Sample Interval: {}ms", config.sample_interval_ms);
    println!("Report Interval: {}ms", config.report_interval_ms);
    
    let mut report = SensorReport::new(config.node_id.clone(), config.report_interval_ms);
    let mut last_report_time = current_time_ms();
    
    loop {
        let now = current_time_ms();
        
        // Collect wind reading
        let reading = generate_simulated_reading(now);
        report.wind_readings.push(reading);
        
        // Check if it's time to send report
        if now - last_report_time >= config.report_interval_ms {
            if report.is_valid() {
                println!("Sending report: {} wind readings, {}g litter",
                    report.wind_readings.len(),
                    report.total_litter_mass_g()
                );
                
                // In production, this would transmit via HTTP/MQTT
                // transmit_report(&report, &config.server_url)?;
                
                report = SensorReport::new(config.node_id.clone(), config.report_interval_ms);
                last_report_time = now;
            }
        }
        
        thread::sleep(Duration::from_millis(config.sample_interval_ms));
    }
}

fn current_time_ms() -> u64 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_millis() as u64
}
