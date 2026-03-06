/**
 * @file AlnBridge.hpp
 * @brief ALN-Blockchain bridge for Windminer record submission
 * 
 * @author Ecotribute Research Team
 * @date 2026
 */

#ifndef ALN_BRIDGE_HPP
#define ALN_BRIDGE_HPP

#include "AlnTypes.hpp"
#include <string>
#include <vector>
#include <memory>

namespace windminer {
namespace aln {

/**
 * @brief ALN Bridge for blockchain interactions
 */
class AlnBridge {
private:
    AlnConfig config_;
    bool connected_;
    std::string last_error_;
    
public:
    /**
     * @brief Default constructor
     */
    AlnBridge();
    
    /**
     * @brief Constructor with configuration
     * @param config ALN configuration
     */
    explicit AlnBridge(const AlnConfig& config);
    
    /**
     * @brief Initialize connection to ALN network
     * @return true if successful
     */
    bool connect();
    
    /**
     * @brief Check if connected to ALN network
     * @return Connection status
     */
    bool isConnected() const { return connected_; }
    
    /**
     * @brief Submit capture record to blockchain
     * @param record Capture record to submit
     * @return ContractResult with transaction details
     */
    ContractResult submitCaptureRecord(const AlnRecord& record);
    
    /**
     * @brief Submit wildlife audit record
     * @param node_id Windnet node identifier
     * @param checklist Wildlife safety checklist
     * @return ContractResult with transaction details
     */
    ContractResult submitWildlifeAudit(
        const std::string& node_id,
        const WildlifeSafetyChecklist& checklist
    );
    
    /**
     * @brief Claim Karma rewards for verified captures
     * @param node_id Windnet node identifier
     * @param period_start Start of claim period (Unix timestamp)
     * @param period_end End of claim period (Unix timestamp)
     * @return ContractResult with transaction details
     */
    ContractResult claimKarma(
        const std::string& node_id,
        uint64_t period_start,
        uint64_t period_end
    );
    
    /**
     * @brief Query verified capture total for a node
     * @param node_id Windnet node identifier
     * @return Total verified capture mass (kg)
     */
    double queryVerifiedCapture(const std::string& node_id);
    
    /**
     * @brief Query total Karma earned for a node
     * @param node_id Windnet node identifier
     * @return Total Karma units
     */
    double queryTotalKarma(const std::string& node_id);
    
    /**
     * @brief Get wildlife compliance status for a node
     * @param node_id Windnet node identifier
     * @return WildlifeSafetyLevel
     */
    WildlifeSafetyLevel queryComplianceStatus(const std::string& node_id);
    
    /**
     * @brief Get last error message
     * @return Error string
     */
    std::string getLastError() const { return last_error_; }
    
    /**
     * @brief Set configuration
     * @param config New ALN configuration
     */
    void setConfig(const AlnConfig& config) { config_ = config; }
    
    /**
     * @brief Get current configuration
     * @return ALN configuration
     */
    const AlnConfig& getConfig() const { return config_; }
};

/**
 * @brief Batch record submission for efficiency
 */
struct BatchSubmission {
    std::vector<AlnRecord> records;
    std::string batch_id;
    uint64_t timestamp;
    
    /**
     * @brief Calculate total Karma for batch
     * @return Total Karma units
     */
    double calculateTotalKarma() const {
        double total = 0.0;
        for (const auto& rec : records) {
            total += rec.final_karma;
        }
        return total;
    }
    
    /**
     * @brief Calculate total capture mass for batch
     * @return Total kg captured
     */
    double calculateTotalCapture() const {
        double total = 0.0;
        for (const auto& rec : records) {
            total += rec.capture_mass_kg;
        }
        return total;
    }
};

/**
 * @brief Submit batch of records to ALN
 * @param bridge ALN bridge instance
 * @param batch Batch submission data
 * @return Vector of ContractResults for each record
 */
std::vector<ContractResult> submitBatchToAln(
    AlnBridge& bridge,
    const BatchSubmission& batch
);

} // namespace aln
} // namespace windminer

#endif // ALN_BRIDGE_HPP
