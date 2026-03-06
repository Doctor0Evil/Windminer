//! Command-line interface for windminer Rust tools

use windminer_rs::*;
use windminer_rs::geometry::*;
use windminer_rs::capture::*;
use windminer_rs::impact::*;
use std::env;
use std::fs::File;
use std::io::BufReader;

fn main() -> anyhow::Result<()> {
    let args: Vec<String> = env::args().collect();
    
    if args.len() < 2 {
        println!("Windminer CLI v{}", VERSION);
        println!("Usage: windminer_cli <command> [options]");
        println!("Commands:");
        println!("  evaluate <canyon_csv>  - Evaluate canyons from CSV");
        println!("  demo                   - Run demonstration");
        println!("  help                   - Show this help");
        return Ok(());
    }
    
    match args[1].as_str() {
        "demo" => run_demo(),
        "help" => {
            println!("Use --help for detailed options");
            Ok(())
        }
        "evaluate" => {
            if args.len() < 3 {
                eprintln!("Error: Missing input file");
                return Ok(());
            }
            evaluate_file(&args[2])
        }
        _ => {
            eprintln!("Unknown command: {}", args[1]);
            Ok(())
        }
    }
}

fn run_demo() -> anyhow::Result<()> {
    println!("Running Windminer Demo...\n");
    
    // Create demo canyon
    let canyon = Canyon::new(
        "PHX-DEMO-001".to_string(),
        33.4484,
        -112.0740,
        30.0,
        20.0,
        0.0,
    )?;
    
    println!("Canyon: {}", canyon.id);
    println!("  Height: {}m, Width: {}m", canyon.height_m, canyon.width_m);
    println!("  Aspect Ratio: {:.2}", canyon.aspect_ratio());
    println!("  Flow Regime: {}", canyon.classify_flow_regime().as_str());
    println!("  Suitable: {}\n", canyon.is_suitable());
    
    // Calculate capture
    let model = CaptureModel::default();
    let capture = model.evaluate_canyon(&canyon);
    
    println!("Capture Model Results:");
    println!("  Expected Capture: {:.1} kg/year", capture.expected_capture_kg_year);
    println!("  Net Area: {:.1} m²", capture.net_area_m2);
    println!("  Wind Hours: {:.0} h/year\n", capture.effective_wind_hours);
    
    // Calculate eco-impact
    let calc = EcoImpactCalculator::default();
    let nuisance = NuisanceFactors::default();
    let eco = calc.evaluate(&capture, &nuisance);
    
    println!("Eco-Impact Results:");
    println!("  Score: {:.2}", eco.eco_impact_score);
    println!("  Recommendation: {}", eco.recommendation);
    println!("  Karma: {:.0} units/year\n", eco.karma_per_unit);
    
    println!("Demo complete!");
    Ok(())
}

fn evaluate_file(filepath: &str) -> anyhow::Result<()> {
    println!("Evaluating canyons from: {}", filepath);
    
    let file = File::open(filepath)?;
    let reader = BufReader::new(file);
    
    let mut rdr = csv::Reader::from_reader(reader);
    let mut count = 0;
    let mut suitable = 0;
    
    for result in rdr.records() {
        let record = result?;
        // Parse CSV record into Canyon struct
        // (Simplified for demo)
        count += 1;
    }
    
    println!("Processed {} canyons", count);
    Ok(())
}
