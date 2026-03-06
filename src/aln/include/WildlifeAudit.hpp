/**
 * @file WildlifeAudit.hpp
 * @brief Wildlife safety audit tools and verification
 * 
 * @author Ecotribute Research Team
 * @date 2026
 */

#ifndef WILDLIFE_AUDIT_HPP
#define WILDLIFE_AUDIT_HPP

#include "AlnTypes.hpp"
#include <string>
#include <vector>
#include <fstream>

namespace windminer {
namespace aln {

/**
 * @brief Wildlife audit evidence file types
 */
enum class EvidenceType {
    PHOTO_BAR_SPACING,
    PHOTO_EDGES,
    PHOTO_ESCAPE_CORRIDOR,
    MATERIAL_CERTIFICATE_ISO14851,
    TOXICITY_REPORT_OECD,
    STRUCTURAL_ENGINEER_STAMP,
    PASSIVE_OPERATION_CHECKLIST,
    MAINTENANCE_LOG
};

/**
 * @brief Evidence file metadata
 */
struct EvidenceFile {
    EvidenceType type;
    std::string filepath;
    std::string ipfs_hash;
    uint64_t upload_timestamp;
    std::string uploader_address;
    bool is_verified;
};

/**
 * @brief Wildlife audit report structure
 */
struct WildlifeAuditReport {
    std::string node_id;
    std::string auditor_name;
    std::string auditor_address;
    uint64_t audit_date;
    WildlifeSafetyChecklist checklist;
    std::vector<EvidenceFile> evidence_files;
    std::string summary_notes;
    WildlifeSafetyLevel compliance_level;
    double karma_multiplier;
    bool is_approved;
    
    /**
     * @brief Generate audit report from checklist
     */
    void generateFromChecklist(const WildlifeSafetyChecklist& checks) {
        checklist = checks;
        compliance_level = checks.calculateCompliance();
        karma_multiplier = checks.getKarmaMultiplier();
        is_approved = (compliance_level != WildlifeSafetyLevel::NON_COMPLIANT);
        audit_date = std::chrono::system_clock::now().time_since_epoch().count();
    }
    
    /**
     * @brief Export report to JSON for IPFS upload
     * @return JSON string
     */
    std::string toJson() const;
    
    /**
     * @brief Import report from JSON
     * @param json JSON string
     * @return true if successful
     */
    bool fromJson(const std::string& json);
};

/**
 * @brief Wildlife audit manager
 */
class WildlifeAuditManager {
private:
    std::string audit_storage_path_;
    
public:
    /**
     * @brief Default constructor
     */
    WildlifeAuditManager();
    
    /**
     * @brief Constructor with storage path
     * @param path Path to store audit files
     */
    explicit WildlifeAuditManager(const std::string& path);
    
    /**
     * @brief Create new audit for a node
     * @param node_id Windnet node identifier
     * @param auditor_name Auditor name
     * @return WildlifeAuditReport
     */
    WildlifeAuditReport createAudit(
        const std::string& node_id,
        const std::string& auditor_name
    );
    
    /**
     * @brief Add evidence file to audit
     * @param report Audit report reference
     * @param type Evidence type
     * @param filepath Path to evidence file
     * @return true if successful
     */
    bool addEvidence(
        WildlifeAuditReport& report,
        EvidenceType type,
        const std::string& filepath
    );
    
    /**
     * @brief Verify evidence file exists and is valid
     * @param evidence Evidence file to verify
     * @return true if valid
     */
    bool verifyEvidence(const EvidenceFile& evidence);
    
    /**
     * @brief Submit audit to ALN blockchain
     * @param bridge ALN bridge instance
     * @param report Audit report
     * @return ContractResult
     */
    ContractResult submitToAln(AlnBridge& bridge, const WildlifeAuditReport& report);
    
    /**
     * @brief Load audit report from file
     * @param filepath Path to audit file
     * @return WildlifeAuditReport
     */
    WildlifeAuditReport loadAudit(const std::string& filepath);
    
    /**
     * @brief Save audit report to file
     * @param report Audit report
     * @param filepath Output path
     * @return true if successful
     */
    bool saveAudit(const WildlifeAuditReport& report, const std::string& filepath);
    
    /**
     * @brief Get all audits for a node
     * @param node_id Node identifier
     * @return Vector of audit reports
     */
    std::vector<WildlifeAuditReport> getNodeAudits(const std::string& node_id);
    
    /**
     * @brief Get latest compliance status for a node
     * @param node_id Node identifier
     * @return WildlifeSafetyLevel
     */
    WildlifeSafetyLevel getLatestCompliance(const std::string& node_id);
};

/**
 * @brief Generate audit checklist template
 * @return WildlifeSafetyChecklist with all false values
 */
inline WildlifeSafetyChecklist generateChecklistTemplate() {
    WildlifeSafetyChecklist checklist;
    checklist.bar_spacing_50mm_plus = false;
    checklist.smooth_rounded_edges = false;
    checklist.clear_escape_corridor = false;
    checklist.no_fine_mesh = false;
    checklist.biodegradable_materials = false;
    checklist.non_toxic_coatings = false;
    checklist.passive_operation = false;
    checklist.structural_rating_asce7 = false;
    return checklist;
}

/**
 * @brief Print checklist to console for manual audit
 * @param checklist Checklist to print
 */
inline void printChecklist(const WildlifeSafetyChecklist& checklist) {
    std::cout << "\n=== Wildlife Safety Audit Checklist ===\n" << std::endl;
    std::cout << "[ ] Bar spacing ≥50mm: " << (checklist.bar_spacing_50mm_plus ? "PASS" : "FAIL") << std::endl;
    std::cout << "[ ] Smooth rounded edges: " << (checklist.smooth_rounded_edges ? "PASS" : "FAIL") << std::endl;
    std::cout << "[ ] Clear escape corridor: " << (checklist.clear_escape_corridor ? "PASS" : "FAIL") << std::endl;
    std::cout << "[ ] No fine mesh <50mm: " << (checklist.no_fine_mesh ? "PASS" : "FAIL") << std::endl;
    std::cout << "[ ] Biodegradable materials (ISO 14851): " << (checklist.biodegradable_materials ? "PASS" : "FAIL") << std::endl;
    std::cout << "[ ] Non-toxic coatings (OECD): " << (checklist.non_toxic_coatings ? "PASS" : "FAIL") << std::endl;
    std::cout << "[ ] Passive operation (no electricity): " << (checklist.passive_operation ? "PASS" : "FAIL") << std::endl;
    std::cout << "[ ] Structural rating (ASCE 7): " << (checklist.structural_rating_asce7 ? "PASS" : "FAIL") << std::endl;
    std::cout << "\nCompliance Level: " << safetyLevelToString(checklist.calculateCompliance()) << std::endl;
    std::cout << "Karma Multiplier: " << checklist.getKarmaMultiplier() << "×\n" << std::endl;
}

} // namespace aln
} // namespace windminer

#endif // WILDLIFE_AUDIT_HPP
