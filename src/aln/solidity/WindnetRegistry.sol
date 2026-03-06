// SPDX-License-Identifier: MIT
pragma solidity ^0.8.19;

/**
 * @title WindnetRegistry
 * @dev ALN smart contract for windnet node registration and management
 * 
 * Wildlife-Safe Governance:
 * - All nodes must pass wildlife safety audit before activation
 * - Non-compliant nodes are suspended (Karma = 0)
 * - Full compliance earns 1.5x Karma multiplier
 */
contract WindnetRegistry {
    
    // Structure for windnet node registration
    struct WindnetNode {
        string nodeId;
        address owner;
        uint256 latitude;      // Fixed point (×1e6)
        uint256 longitude;     // Fixed point (×1e6)
        uint256 registrationDate;
        bool isActive;
        bool isWildlifeCompliant;
        uint256 lastAuditDate;
        string evidenceHash;   // IPFS hash of compliance documents
    }
    
    // Structure for wildlife safety audit
    struct WildlifeAudit {
        string nodeId;
        address auditor;
        uint256 timestamp;
        bool barSpacing50mm;
        bool smoothEdges;
        bool escapeCorridor;
        bool noFineMesh;
        bool biodegradableMaterials;
        bool nonToxicCoatings;
        bool passiveOperation;
        bool structuralRating;
        uint8 complianceScore; // 0-8 (number of checks passed)
        bool isApproved;
    }
    
    // Structure for capture record
    struct CaptureRecord {
        string nodeId;
        uint256 timestamp;
        uint256 captureMassKg;  // Fixed point (×1e3)
        uint256 ecoImpactScore; // Fixed point (×1e6)
        address verifier;
        bool isVerified;
        uint256 karmaAwarded;
    }
    
    // State variables
    mapping(string => WindnetNode) public nodes;
    mapping(string => WildlifeAudit[]) public nodeAudits;
    mapping(string => CaptureRecord[]) public nodeCaptures;
    mapping(string => uint256) public nodeTotalKarma;
    mapping(string => uint256) public nodeTotalCapture;
    mapping(address => string[]) public ownerNodes;
    
    // Events
    event NodeRegistered(string nodeId, address owner, uint256 timestamp);
    event NodeSuspended(string nodeId, string reason);
    event NodeReactivated(string nodeId);
    event AuditSubmitted(string nodeId, address auditor, uint256 score, bool approved);
    event CaptureRecorded(string nodeId, uint256 mass, uint256 karma);
    event KarmaClaimed(string nodeId, address owner, uint256 amount);
    
    // Constants
    uint256 public constant KARMA_MULTIPLIER_FULL = 150;  // 1.5× (basis points)
    uint256 public constant KARMA_MULTIPLIER_PARTIAL = 100; // 1.0×
    uint256 public constant KARMA_MULTIPLIER_NONE = 0;      // 0.0×
    uint256 public constant COMPLIANCE_THRESHOLD = 6;       // 6/8 checks for partial
    
    /**
     * @dev Register a new windnet node
     * @param _nodeId Unique node identifier
     * @param _latitude Latitude (×1e6)
     * @param _longitude Longitude (×1e6)
     * @param _evidenceHash IPFS hash of registration documents
     */
    function registerNode(
        string calldata _nodeId,
        uint256 _latitude,
        uint256 _longitude,
        string calldata _evidenceHash
    ) external {
        require(bytes(nodes[_nodeId].nodeId).length == 0, "Node already exists");
        
        nodes[_nodeId] = WindnetNode({
            nodeId: _nodeId,
            owner: msg.sender,
            latitude: _latitude,
            longitude: _longitude,
            registrationDate: block.timestamp,
            isActive: false,  // Inactive until wildlife audit passes
            isWildlifeCompliant: false,
            lastAuditDate: 0,
            evidenceHash: _evidenceHash
        });
        
        ownerNodes[msg.sender].push(_nodeId);
        
        emit NodeRegistered(_nodeId, msg.sender, block.timestamp);
    }
    
    /**
     * @dev Submit wildlife safety audit
     * @param _nodeId Node identifier
     * @param _audit Audit checklist data
     */
    function submitWildlifeAudit(
        string calldata _nodeId,
        bool[8] calldata _checks
    ) external {
        require(bytes(nodes[_nodeId].nodeId).length > 0, "Node not found");
        require(nodes[_nodeId].owner == msg.sender, "Not node owner");
        
        uint8 score = 0;
        for (uint8 i = 0; i < 8; i++) {
            if (_checks[i]) {
                score++;
            }
        }
        
        bool approved = score >= COMPLIANCE_THRESHOLD;
        bool fullCompliance = score == 8;
        
        WildlifeAudit memory audit = WildlifeAudit({
            nodeId: _nodeId,
            auditor: msg.sender,
            timestamp: block.timestamp,
            barSpacing50mm: _checks[0],
            smoothEdges: _checks[1],
            escapeCorridor: _checks[2],
            noFineMesh: _checks[3],
            biodegradableMaterials: _checks[4],
            nonToxicCoatings: _checks[5],
            passiveOperation: _checks[6],
            structuralRating: _checks[7],
            complianceScore: score,
            isApproved: approved
        });
        
        nodeAudits[_nodeId].push(audit);
        nodes[_nodeId].lastAuditDate = block.timestamp;
        nodes[_nodeId].isWildlifeCompliant = approved;
        
        // Activate node if first passing audit
        if (approved && !nodes[_nodeId].isActive) {
            nodes[_nodeId].isActive = true;
        }
        
        emit AuditSubmitted(_nodeId, msg.sender, score, approved);
    }
    
    /**
     * @dev Record verified capture data
     * @param _nodeId Node identifier
     * @param _captureMassKg Capture mass (×1e3 for precision)
     * @param _ecoImpactScore Eco-impact score (×1e6 for precision)
     * @param _karmaAwarded Karma units awarded
     */
    function recordCapture(
        string calldata _nodeId,
        uint256 _captureMassKg,
        uint256 _ecoImpactScore,
        uint256 _karmaAwarded
    ) external {
        require(nodes[_nodeId].isActive, "Node not active");
        require(nodes[_nodeId].isWildlifeCompliant, "Node not wildlife compliant");
        
        CaptureRecord memory record = CaptureRecord({
            nodeId: _nodeId,
            timestamp: block.timestamp,
            captureMassKg: _captureMassKg,
            ecoImpactScore: _ecoImpactScore,
            verifier: msg.sender,
            isVerified: true,
            karmaAwarded: _karmaAwarded
        });
        
        nodeCaptures[_nodeId].push(record);
        nodeTotalKarma[_nodeId] += _karmaAwarded;
        nodeTotalCapture[_nodeId] += _captureMassKg;
        
        emit CaptureRecorded(_nodeId, _captureMassKg, _karmaAwarded);
    }
    
    /**
     * @dev Get Karma multiplier for a node
     * @param _nodeId Node identifier
     * @return Multiplier in basis points (100 = 1.0×, 150 = 1.5×)
     */
    function getKarmaMultiplier(string calldata _nodeId) 
        external 
        view 
        returns (uint256) 
    {
        require(bytes(nodes[_nodeId].nodeId).length > 0, "Node not found");
        
        if (!nodes[_nodeId].isWildlifeCompliant) {
            return KARMA_MULTIPLIER_NONE;
        }
        
        WildlifeAudit[] memory audits = nodeAudits[_nodeId];
        if (audits.length == 0) {
            return KARMA_MULTIPLIER_NONE;
        }
        
        WildlifeAudit memory latest = audits[audits.length - 1];
        if (latest.complianceScore == 8) {
            return KARMA_MULTIPLIER_FULL;
        } else {
            return KARMA_MULTIPLIER_PARTIAL;
        }
    }
    
    /**
     * @dev Suspend a non-compliant node
     * @param _nodeId Node identifier
     * @param _reason Suspension reason
     */
    function suspendNode(string calldata _nodeId, string calldata _reason) 
        external 
    {
        require(bytes(nodes[_nodeId].nodeId).length > 0, "Node not found");
        // Only owner or governance can suspend
        require(
            msg.sender == nodes[_nodeId].owner || 
            msg.sender == governanceAddress, 
            "Not authorized"
        );
        
        nodes[_nodeId].isActive = false;
        emit NodeSuspended(_nodeId, _reason);
    }
    
    // Governance address (set during deployment)
    address public governanceAddress;
    
    constructor(address _governance) {
        governanceAddress = _governance;
    }
}
