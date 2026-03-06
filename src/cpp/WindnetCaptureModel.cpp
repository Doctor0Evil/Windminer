/**
 * @file WindnetCaptureModel.cpp
 * @brief Implementation of physics-based litter capture model
 */

#include "WindnetCaptureModel.hpp"
#include <cmath>
#include <iostream>

namespace windminer {
namespace model {

/**
 * @brief Calculate expected annual litter capture
 * Formula: M_cap = Cℓ* · Uimp* · Anet · ηnet · Twind
 */
double CaptureModel::calculateCapture(
    const geometry::Canyon& canyon,
    const geometry::Windnet& net
) const {
    // Check suitability first
    if (!canyon.isSuitableForWindnet()) {
        return 0.0;
    }
    
    // Convert wind time from seconds to hours for consistency if needed
    // Here we use seconds in formula as per research definition
    double twind_seconds = params_.effective_wind_seconds;
    
    // Calculate capture mass
    double capture = params_.litter_concentration * 
                     params_.impingement_speed * 
                     net.area() * 
                     params_.net_efficiency * 
                     twind_seconds;
    
    return capture; // kg/year
}

/**
 * @brief Static calculation helper
 */
double CaptureModel::calculateCapture(
    const geometry::Canyon& canyon,
    const geometry::Windnet& net,
    const CaptureParameters& params
) {
    CaptureModel model(params);
    return model.calculateCapture(canyon, net);
}

/**
 * @brief Evaluate full capture result for a canyon
 * Includes geometry validation and flow regime classification
 */
CaptureResult CaptureModel::evaluateCanyon(const geometry::Canyon& canyon) const {
    CaptureResult result;
    result.canyon_id = canyon.id;
    result.flow_regime = geometry::flowRegimeToString(canyon.classifyFlowRegime());
    result.is_suitable = canyon.isSuitableForWindnet();
    
    if (!result.is_suitable) {
        result.expected_capture_kg_year = 0.0;
        result.net_area_m2 = 0.0;
        result.effective_wind_hours = 0.0;
        return result;
    }
    
    // Generate optimal net placement
    geometry::Windnet net = geometry::calculateOptimalPlacement(canyon);
    
    result.net_area_m2 = net.area();
    result.effective_wind_hours = params_.effective_wind_seconds / 3600.0;
    result.expected_capture_kg_year = calculateCapture(canyon, net);
    
    return result;
}

/**
 * @brief Perform sensitivity analysis on capture model
 * Tests how ±10% change in parameters affects output
 */
std::vector<SensitivityResult> performSensitivityAnalysis(
    const geometry::Canyon& canyon,
    const geometry::Windnet& net,
    const CaptureParameters& params
) {
    std::vector<SensitivityResult> results;
    CaptureModel base_model(params);
    double base_capture = base_model.calculateCapture(canyon, net);
    
    // Helper lambda to test parameter sensitivity
    auto test_param = [&](const std::string& name, double base_val, double& storage) {
        CaptureParameters perturbed = params;
        double perturbed_val = base_val * 1.10; // +10%
        storage = perturbed_val;
        
        CaptureModel perturbed_model(perturbed);
        double perturbed_capture = perturbed_model.calculateCapture(canyon, net);
        
        SensitivityResult res;
        res.parameter_name = name;
        res.baseline_value = base_val;
        res.perturbed_value = perturbed_val;
        res.baseline_capture = base_capture;
        res.perturbed_capture = perturbed_capture;
        
        // Sensitivity coefficient = (% change output) / (% change input)
        double pct_change_in = (perturbed_val - base_val) / base_val;
        double pct_change_out = (perturbed_capture - base_capture) / base_capture;
        res.sensitivity_coefficient = pct_change_out / pct_change_in;
        
        results.push_back(res);
        
        // Reset storage
        storage = base_val;
    };
    
    // Note: Since params are passed by value in struct, we modify copies
    // For demonstration, we manually create perturbed params
    
    // 1. Litter Concentration
    {
        CaptureParameters p = params;
        p.litter_concentration *= 1.10;
        CaptureModel m(p);
        double cap = m.calculateCapture(canyon, net);
        results.push_back({"litter_concentration", params.litter_concentration, 
                          p.litter_concentration, base_capture, cap, 1.0});
    }
    
    // 2. Net Efficiency
    {
        CaptureParameters p = params;
        p.net_efficiency *= 1.10;
        CaptureModel m(p);
        double cap = m.calculateCapture(canyon, net);
        results.push_back({"net_efficiency", params.net_efficiency, 
                          p.net_efficiency, base_capture, cap, 1.0});
    }
    
    return results;
}

} // namespace model
} // namespace windminer
