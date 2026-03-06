/**
 * @file WindnetCaptureModel.hpp
 * @brief Physics-based litter capture model for windnets
 * 
 * Implements the core capture equation:
 * M_cap ≈ Cℓ* · Uimp* · Anet · ηnet · Twind
 * 
 * @author Ecotribute Research Team
 * @date 2026
 */

#ifndef WINDNET_CAPTURE_MODEL_HPP
#define WINDNET_CAPTURE_MODEL_HPP

#include "WindnetGeometry.hpp"
#include "WindClimatePhoenix.hpp"
#include <string>

namespace windminer {
namespace model {

/**
 * @brief Capture model parameters (Phoenix-specific defaults)
 */
struct CaptureParameters {
    double litter_concentration;    ///< Cℓ* (kg/m³) - default: 1.0e-8
    double impingement_speed;       ///< Uimp* (m/s) - default: 2.5
    double net_efficiency;          ///< ηnet (0.0-1.0) - default: 0.4
    double effective_wind_seconds;  ///< Twind (seconds) - default: 7.2e6 (2000 hours)
    double reference_capture_kg;    ///< Mref for eco-score (kg/year) - default: 150
    
    /**
     * @brief Default constructor with Phoenix baseline values
     */
    CaptureParameters()
        : litter_concentration(1.0e-8)
        , impingement_speed(2.5)
        , net_efficiency(0.4)
        , effective_wind_seconds(7.2e6)
        , reference_capture_kg(150.0)
    {}
};

/**
 * @brief Capture model result structure
 */
struct CaptureResult {
    std::string canyon_id;
    double expected_capture_kg_year;    ///< M_cap (kg/year)
    double net_area_m2;                  ///< Anet (m²)
    double effective_wind_hours;         ///< Twind (hours)
    bool is_suitable;                    ///< Suitability flag
    std::string flow_regime;             ///< Flow regime classification
};

/**
 * @brief Windnet capture model calculator
 */
class CaptureModel {
private:
    CaptureParameters params_;
    climate::PhoenixWindClimate wind_climate_;
    
public:
    /**
     * @brief Default constructor
     */
    CaptureModel() = default;
    
    /**
     * @brief Constructor with custom parameters
     * @param params Capture parameters
     */
    explicit CaptureModel(const CaptureParameters& params)
        : params_(params)
    {}
    
    /**
     * @brief Calculate expected annual litter capture
     * 
     * Formula: M_cap = Cℓ* · Uimp* · Anet · ηnet · Twind
     * 
     * @param canyon Reference canyon geometry
     * @param net Windnet specifications
     * @return Expected capture mass (kg/year)
     */
    double calculateCapture(
        const geometry::Canyon& canyon,
        const geometry::Windnet& net
    ) const;
    
    /**
     * @brief Calculate capture with custom parameters
     * @param canyon Reference canyon
     * @param net Windnet specifications
     * @param params Custom capture parameters
     * @return Expected capture mass (kg/year)
     */
    static double calculateCapture(
        const geometry::Canyon& canyon,
        const geometry::Windnet& net,
        const CaptureParameters& params
    );
    
    /**
     * @brief Evaluate full capture result for a canyon
     * @param canyon Reference canyon
     * @return CaptureResult struct with all metrics
     */
    CaptureResult evaluateCanyon(const geometry::Canyon& canyon) const;
    
    /**
     * @brief Get current parameters
     * @return CaptureParameters struct
     */
    const CaptureParameters& getParameters() const {
        return params_;
    }
    
    /**
     * @brief Set custom parameters
     * @param params New capture parameters
     */
    void setParameters(const CaptureParameters& params) {
        params_ = params;
    }
};

/**
 * @brief Sensitivity analysis result
 */
struct SensitivityResult {
    std::string parameter_name;
    double baseline_value;
    double perturbed_value;
    double baseline_capture;
    double perturbed_capture;
    double sensitivity_coefficient;  ///< % change in output / % change in input
};

/**
 * @brief Perform sensitivity analysis on capture model
 * @param canyon Reference canyon
 * @param net Windnet specifications
 * @param params Base parameters
 * @return Vector of SensitivityResult for each parameter
 */
std::vector<SensitivityResult> performSensitivityAnalysis(
    const geometry::Canyon& canyon,
    const geometry::Windnet& net,
    const CaptureParameters& params
);

} // namespace model
} // namespace windminer

#endif // WINDNET_CAPTURE_MODEL_HPP
