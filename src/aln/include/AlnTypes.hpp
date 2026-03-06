/**
 * @file AlnTypes.hpp
 * @brief ALN-Blockchain type definitions for Windminer governance
 * 
 * @author Ecotribute Research Team
 * @date 2026
 */

#ifndef ALN_TYPES_HPP
#define ALN_TYPES_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace windminer {
namespace aln {

/**
 * @brief Wildlife safety compliance levels
 */
enum class WildlifeSafetyLevel {
    NON_COMPLIANT,      ///< Does not meet safety standards (Karma = 0)
    PARTIAL_COMPLIANCE, ///< Some requirements met (Karma multiplier = 1.0)
    FULL_COMPLIANCE     ///< All requirements met (Karma multiplier = 1.5)
};

/**
 * @brief Wildlife safety verification checklist
 */
struct WildlifeSafetyChecklist {
    bool bar_spacing_50mm_plus;      ///< ≥50mm bar spacing
    bool smooth_rounded_edges;       ///< No sharp edges or snag points
    bool clear_escape_corridor;      ///< Full-height flight path
    bool no_fine_mesh;               ///< No mesh <50mm
    bool biodegradable_materials;    ///< ISO 14851 certified
    bool non_toxic_coatings;         ///< OECD toxicity passed
    bool passive_operation;          ///< No electricity required
    bool structural_rating_asce7;    ///< Phoenix wind code compliant
    
    /**
     * @brief Calculate compliance level
     * @return WildlifeSafetyLevel enum
     */
    WildlifeSafetyLevel calculateCompliance() const {
        int passed = 0;
        int total = 8;
        
        if (bar_spacing_50mm_plus) passed++;
        if (smooth_rounded_edges) passed++;
        if (clear_escape_corridor) passed++;
        if (no_fine_mesh) passed++;
        if (biodegradable_materials) passed++;
        if (non_toxic_coatings) passed++;
        if (passive_operation) passed++;
        if (structural_rating_asce7) passed++;
        
        if (passed == total) {
            return WildlifeSafetyLevel::FULL_COMPLIANCE;
        } else if (passed >= 6) {
            return WildlifeSafetyLevel::PARTIAL_COMPLIANCE;
        } else {
            return WildlifeSafetyLevel::NON_COMPLIANT;
        }
    }
    
    /**
     * @brief Get Karma multiplier for compliance level
     * @return Multiplier value (0.0, 1.0, or 1.5)
     */
    double getKarmaMultiplier() const {
        WildlifeSafetyLevel level = calculateCompliance();
        switch (level) {
            case WildlifeSafetyLevel::FULL_COMPLIANCE:
                return 1.5;
            case WildlifeSafetyLevel::PARTIAL_COMPLIANCE:
                return 1.0;
            case WildlifeSafetyLevel::NON_COMPLIANT:
            default:
                return 0.0;
        }
    }
};

/**
 * @brief ALN blockchain record structure
 */
struct AlnRecord {
    std::string record_id;              ///< Unique record identifier
    std::string node_id;                ///< Windnet node identifier
    uint64_t timestamp;                 ///< Unix timestamp (seconds)
    std::string record_type;            ///< "capture", "audit", "karma"
    
    // Capture data
    double capture_mass_kg;             ///< Verified capture mass (kg)
    double eco_impact_score;            ///< Eco-impact score (0-1)
    
    // Wildlife safety
    WildlifeSafetyChecklist safety_checklist;
    WildlifeSafetyLevel safety_level;
    double karma_multiplier;
    
    // Karma calculation
    double base_karma;                  ///< Before multiplier
    double final_karma;                 ///< After multiplier
    
    // Verification
    std::string verifier_address;       ///< ALN address of verifier
    std::string evidence_hash;          ///< IPFS hash of evidence files
    bool is_verified;                   ///< Blockchain verification status
    
    /**
     * @brief Calculate Karma with wildlife safety multiplier
     */
    void calculateKarma() {
        safety_level = safety_checklist.calculateCompliance();
        karma_multiplier = safety_checklist.getKarmaMultiplier();
        
        base_karma = capture_mass_kg * eco_impact_score * 10.0; // 10 Karma per kg at score 1.0
        final_karma = base_karma * karma_multiplier;
    }
};

/**
 * @brief Smart contract interaction result
 */
struct ContractResult {
    bool success;
    std::string transaction_hash;
    std::string error_message;
    uint64_t block_number;
    uint64_t gas_used;
};

/**
 * @brief ALN node configuration
 */
struct AlnConfig {
    std::string rpc_endpoint;           ///< ALN RPC URL
    std::string contract_registry;      ///< WindnetRegistry address
    std::string contract_verifier;      ///< CaptureVerifier address
    std::string contract_karma;         ///< KarmaDistributor address
    std::string contract_audit;         ///< WildlifeAudit address
    std::string wallet_address;         ///< Node wallet address
    std::string wallet_private_key;     ///< Encrypted private key
    int confirmation_blocks;            ///< Blocks to wait for confirmation
    
    /**
     * @brief Default Phoenix ALN testnet configuration
     */
    static AlnConfig phoenixTestnet() {
        AlnConfig config;
        config.rpc_endpoint = "https://testnet.aln.ecotribute.org/rpc";
        config.contract_registry = "0xWND000000000000000000000000000000001";
        config.contract_verifier = "0xWND000000000000000000000000000000002";
        config.contract_karma = "0xWND000000000000000000000000000000003";
        config.contract_audit = "0xWND000000000000000000000000000000004";
        config.confirmation_blocks = 6;
        return config;
    }
    
    /**
     * @brief Default Phoenix ALN mainnet configuration
     */
    static AlnConfig phoenixMainnet() {
        AlnConfig config = phoenixTestnet();
        config.rpc_endpoint = "https://mainnet.aln.ecotribute.org/rpc";
        return config;
    }
};

/**
 * @brief Convert WildlifeSafetyLevel to string
 */
inline std::string safetyLevelToString(WildlifeSafetyLevel level) {
    switch (level) {
        case WildlifeSafetyLevel::NON_COMPLIANT:
            return "NON_COMPLIANT";
        case WildlifeSafetyLevel::PARTIAL_COMPLIANCE:
            return "PARTIAL_COMPLIANCE";
        case WildlifeSafetyLevel::FULL_COMPLIANCE:
            return "FULL_COMPLIANCE";
        default:
            return "UNKNOWN";
    }
}

} // namespace aln
} // namespace windminer

#endif // ALN_TYPES_HPP
