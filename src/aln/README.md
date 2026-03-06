# ALN-Blockchain Integration

## Overview

This module provides blockchain-verified governance tracking for Windminer windnet deployments. All capture data, eco-impact scores, and Karma units are recorded on the ALN (Application Library for Nodes) blockchain for transparent, auditable environmental impact tracking.

## Wildlife-Safe Governance

Following the wildlife-safe design principles, all windnet nodes must verify:

| Requirement | Standard | Verification |
|-------------|----------|--------------|
| Bar Spacing | ≥50mm | Photo + measurement upload |
| Material Safety | ISO 14851 | Certificate upload |
| Toxicity | OECD Guidelines | Lab report upload |
| Structural Rating | ASCE 7 Phoenix | Engineer stamp |
| Passive Operation | No electricity | Inspection checklist |

## Karma Calculation (Updated)

Karma is now weighted by wildlife safety compliance:

```
Karma = (CaptureMass × EcoScore × WildlifeSafetyMultiplier) / 100

Where:
- WildlifeSafetyMultiplier = 1.5 (full compliance)
- WildlifeSafetyMultiplier = 1.0 (partial compliance)
- WildlifeSafetyMultiplier = 0.0 (non-compliant, node suspended)
```

## Smart Contract Addresses

| Contract | Purpose | Address |
|----------|---------|---------|
| WindnetRegistry | Node registration | 0xWND...001 |
| CaptureVerifier | Data validation | 0xWND...002 |
| KarmaDistributor | Reward allocation | 0xWND...003 |
| WildlifeAudit | Safety compliance | 0xWND...004 |

## Integration Points

- C++ Core → ALN FFI → Blockchain Records
- Rust Sensor → ALN API → Real-time Updates
- City Dashboard → ALN Query → Public Verification
```
