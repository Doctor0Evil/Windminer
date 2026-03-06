// SPDX-License-Identifier: MIT
pragma solidity ^0.8.19;

/**
 * @title KarmaDistributor
 * @dev ALN smart contract for Karma reward distribution
 * 
 * Wildlife-Safe Karma Calculation:
 * - Base Karma = capture_mass × eco_score × 10
 * - Final Karma = Base Karma × wildlife_multiplier
 * - Full compliance = 1.5×, Partial = 1.0×, Non-compliant = 0×
 */
contract KarmaDistributor {
    
    // Structure for Karma claim
    struct KarmaClaim {
        string nodeId;
        address claimant;
        uint256 amount;
        uint256 timestamp;
        uint256 periodStart;
        uint256 periodEnd;
        bool isPaid;
    }
    
    // State variables
    mapping(string => KarmaClaim[]) public nodeClaims;
    mapping(address => uint256) public pendingKarma;
    mapping(address => uint256) public paidKarma;
    mapping(string => bool) public claimProcessed;
    
    // Linked to WindnetRegistry
    address public registryAddress;
    
    // Events
    event KarmaClaimed(string nodeId, address claimant, uint256 amount);
    event KarmaPaid(address recipient, uint256 amount);
    event ClaimRejected(string nodeId, string reason);
    
    // Constants
    uint256 public constant BASE_KARMA_FACTOR = 10;  // 10 Karma per kg at score 1.0
    
    /**
     * @dev Calculate Karma for a capture record
     * @param _nodeId Node identifier
     * @param _captureMassKg Capture mass (×1e3 for precision)
     * @param _ecoImpactScore Eco-impact score (×1e6 for precision)
     * @return Karma units (×1e6 for precision)
     */
    function calculateKarma(
        string calldata _nodeId,
        uint256 _captureMassKg,
        uint256 _ecoImpactScore
    ) external view returns (uint256) {
        // Get multiplier from registry
        (bool success, bytes memory data) = registryAddress.staticcall(
            abi.encodeWithSignature("getKarmaMultiplier(string)", _nodeId)
        );
        
        uint256 multiplier = 100; // Default 1.0×
        if (success && data.length >= 32) {
            multiplier = abi.decode(data, (uint256));
        }
        
        // Base Karma = mass × score × factor
        uint256 baseKarma = (_captureMassKg * _ecoImpactScore * BASE_KARMA_FACTOR) / 1e9;
        
        // Apply wildlife multiplier
        uint256 finalKarma = (baseKarma * multiplier) / 100;
        
        return finalKarma;
    }
    
    /**
     * @dev Submit Karma claim for a period
     * @param _nodeId Node identifier
     * @param _periodStart Start timestamp
     * @param _periodEnd End timestamp
     */
    function claimKarma(
        string calldata _nodeId,
        uint256 _periodStart,
        uint256 _periodEnd
    ) external {
        string memory claimKey = keccak256(
            abi.encodePacked(_nodeId, _periodStart, _periodEnd)
        );
        
        require(!claimProcessed[claimKey], "Claim already processed");
        require(_periodEnd > _periodStart, "Invalid period");
        require(_periodEnd <= block.timestamp, "Period not ended");
        
        // Calculate total Karma for period (would query from Registry)
        uint256 totalKarma = _calculatePeriodKarma(_nodeId, _periodStart, _periodEnd);
        
        require(totalKarma > 0, "No Karma to claim");
        
        KarmaClaim memory claim = KarmaClaim({
            nodeId: _nodeId,
            claimant: msg.sender,
            amount: totalKarma,
            timestamp: block.timestamp,
            periodStart: _periodStart,
            periodEnd: _periodEnd,
            isPaid: false
        });
        
        nodeClaims[_nodeId].push(claim);
        pendingKarma[msg.sender] += totalKarma;
        claimProcessed[claimKey] = true;
        
        emit KarmaClaimed(_nodeId, msg.sender, totalKarma);
    }
    
    /**
     * @dev Pay out pending Karma to claimant
     * @param _amount Amount to pay
     */
    function payKarma(uint256 _amount) external {
        require(pendingKarma[msg.sender] >= _amount, "Insufficient pending Karma");
        
        pendingKarma[msg.sender] -= _amount;
        paidKarma[msg.sender] += _amount;
        
        emit KarmaPaid(msg.sender, _amount);
    }
    
    /**
     * @dev Internal function to calculate period Karma
     */
    function _calculatePeriodKarma(
        string memory _nodeId,
        uint256 _start,
        uint256 _end
    ) internal view returns (uint256) {
        // Would query Registry for all captures in period
        // Simplified for contract structure
        return 0;
    }
    
    /**
     * @dev Set Registry address
     * @param _registry Registry contract address
     */
    function setRegistry(address _registry) external {
        require(registryAddress == address(0), "Registry already set");
        registryAddress = _registry;
    }
    
    /**
     * @dev Get pending Karma for an address
     * @param _addr Address to query
     * @return Pending Karma amount
     */
    function getPendingKarma(address _addr) external view returns (uint256) {
        return pendingKarma[_addr];
    }
    
    /**
     * @dev Get total paid Karma for an address
     * @param _addr Address to query
     * @return Paid Karma amount
     */
    function getPaidKarma(address _addr) external view returns (uint256) {
        return paidKarma[_addr];
    }
}
