/**
 * @file MainPlanDemo.cpp
 * @brief Command-line tool for windnet site planning
 * 
 * Usage:
 *   ./windminer_plan --input canyons.csv --output results.csv
 */

#include "WindnetGeometry.hpp"
#include "WindClimatePhoenix.hpp"
#include "WindnetCaptureModel.hpp"
#include "WindnetEcoImpact.hpp"
#include "ShardIo.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

/**
 * @brief Simple argument parser
 */
struct Args {
    std::string input_file;
    std::string output_file;
    bool show_help = false;
};

Args parseArgs(int argc, char* argv[]) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--input" && i + 1 < argc) {
            args.input_file = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            args.output_file = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            args.show_help = true;
        }
    }
    return args;
}

void printHelp() {
    std::cout << "Windminer Plan Demo\n";
    std::cout << "Usage: ./windminer_plan --input <canyons.csv> --output <results.csv>\n";
    std::cout << "Options:\n";
    std::cout << "  --input   Path to input canyon geometry CSV\n";
    std::cout << "  --output  Path to output qpudatashard CSV\n";
    std::cout << "  --help    Show this help message\n";
}

int main(int argc, char* argv[]) {
    Args args = parseArgs(argc, argv);
    
    if (args.show_help) {
        printHelp();
        return 0;
    }
    
    if (args.input_file.empty() || args.output_file.empty()) {
        std::cerr << "Error: Missing input or output file.\n";
        printHelp();
        return 1;
    }
    
    std::cout << "Windminer Planning Tool v1.0\n";
    std::cout << "----------------------------\n";
    
    // 1. Load Canyon Data
    std::cout << "Loading canyon data from: " << args.input_file << "\n";
    std::vector<windminer::geometry::Canyon> canyons = 
        windminer::io::ShardIo::readCanyons(args.input_file);
    
    if (canyons.empty()) {
        std::cerr << "Error: No canyons loaded.\n";
        return 1;
    }
    std::cout << "Loaded " << canyons.size() << " canyon segments.\n";
    
    // 2. Initialize Models
    windminer::climate::PhoenixWindClimate climate;
    windminer::model::CaptureModel capture_model;
    windminer::impact::EcoImpactCalculator eco_calculator;
    
    // 3. Process Each Canyon
    std::vector<windminer::io::QpuDataShard> shards;
    
    for (const auto& canyon : canyons) {
        // Evaluate Capture
        windminer::model::CaptureResult capture = capture_model.evaluateCanyon(canyon);
        
        // Define Nuisance (Default low nuisance for demo)
        windminer::impact::NuisanceFactors nuisance;
        
        // Evaluate Eco-Impact
        windminer::impact::EcoImpactResult eco = eco_calculator.evaluate(capture, nuisance);
        
        // Generate Net Geometry
        windminer::geometry::Windnet net = 
            windminer::geometry::calculateOptimalPlacement(canyon);
        
        // Create Shard Record
        if (eco.isRecommended()) {
            windminer::io::QpuDataShard shard = 
                windminer::io::ShardIo::ecoResultToShard(eco, canyon, net);
            shards.push_back(shard);
            
            std::cout << "Site " << canyon.id 
                      << ": Score=" << eco.eco_impact_score 
                      << ", Capture=" << eco.capture_mass_kg_year << " kg/year\n";
        }
    }
    
    // 4. Write Output
    std::cout << "\nWriting " << shards.size() << " recommended sites to: " 
              << args.output_file << "\n";
    
    if (windminer::io::ShardIo::writeShards(args.output_file, shards)) {
        std::cout << "Success! Plan generated.\n";
        return 0;
    } else {
        std::cerr << "Error: Failed to write output.\n";
        return 1;
    }
}
