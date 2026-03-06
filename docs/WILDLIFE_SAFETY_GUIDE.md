# Wildlife Safety Guide for Windnet Deployments

## Core Principle

**Windnets must be wildlife-permeable "trash sieves" with built-in escape paths, NOT true nets.**

All litter capture must be achieved without trapping, injuring, or killing birds, bats, or insects.

## Design Requirements

### 1. Bar Spacing (CRITICAL)

| Requirement | Specification | Verification |
|-------------|---------------|--------------|
| Minimum spacing | ≥50mm between bars | Caliper measurement + photo |
| Maximum bar diameter | ≤10mm | Reduces snag risk |
| Edge treatment | Rounded, smooth | Visual inspection |

**Why 50mm?**
- Small birds (sparrows, finches) can pass through safely
- Pigeons and crows are excluded but not trapped
- Insects easily transit centimetre-scale openings
- Reference: Bird netting standards for humane exclusion

### 2. No Fine Mesh

| Prohibited | Allowed |
|------------|---------|
| Mesh <50mm | Coarse cage structure |
| Fishing line filaments | Smooth tubular members |
| Tensioned netting | Rigid perforated panels |
| Dangling strands | Fixed, secured elements |

### 3. Escape Corridors

Every windnet unit MUST have:
- Clear vertical flight path (full height)
- No dead-end chambers
- Direct exit route in flight direction
- Minimum 1m² clear opening per 10m² of structure

### 4. Material Safety

| Standard | Requirement | Documentation |
|----------|-------------|---------------|
| ISO 14851 | Ultimate aerobic biodegradability | Certificate upload |
| OECD Toxicity | Non-toxic to aquatic life | Lab report upload |
| ASCE 7 | Phoenix wind code compliance | Engineer stamp |

### 5. Passive Operation

- NO electricity required
- NO moving parts that could injure wildlife
- NO sticky or electrostatic surfaces
- Manual clean-out only (weekly schedule)

## Audit Checklist

All windnet nodes must pass wildlife safety audit BEFORE activation:

```
□ Bar spacing ≥50mm (all measurements documented)
□ Smooth rounded edges (no sharp points)
□ Clear escape corridor (full height, verified)
□ No fine mesh <50mm (entire structure)
□ Biodegradable materials (ISO 14851 cert)
□ Non-toxic coatings (OECD test report)
□ Passive operation (no electrical components)
□ Structural rating (ASCE 7 Phoenix wind code)
```

**Scoring:**
- 8/8 = FULL_COMPLIANCE (1.5× Karma multiplier)
- 6-7/8 = PARTIAL_COMPLIANCE (1.0× Karma multiplier)
- <6/8 = NON_COMPLIANT (0× Karma, node suspended)

## Karma Implications

Wildlife safety directly affects Karma rewards:

```
Base Karma = capture_mass_kg × eco_score × 10
Final Karma = Base Karma × wildlife_multiplier

Where:
- FULL_COMPLIANCE = 1.5× multiplier
- PARTIAL_COMPLIANCE = 1.0× multiplier
- NON_COMPLIANT = 0.0× multiplier (no rewards)
```

**Example:**
- 100 kg capture, 0.85 eco-score
- Base Karma = 100 × 0.85 × 10 = 850
- Full Compliance = 850 × 1.5 = 1,275 Karma
- Non-Compliant = 850 × 0.0 = 0 Karma (node suspended)

## Evidence Requirements

All audits must include:

1. **Photos** (with scale reference):
   - Bar spacing measurements
   - Edge detail shots
   - Escape corridor view
   - Full structure overview

2. **Certificates**:
   - ISO 14851 biodegradability
   - OECD toxicity test results
   - ASCE 7 structural engineer stamp

3. **Documentation**:
   - Maintenance log (weekly clean-outs)
   - Incident report (any wildlife interactions)
   - Material specification sheet

## Audit Frequency

| Audit Type | Frequency | Required For |
|------------|-----------|--------------|
| Initial | Before activation | All new nodes |
| Annual | Every 12 months | Compliance renewal |
| Incident | Within 48 hours | Any wildlife interaction |
| Random | Quarterly | Governance verification |

## Non-Compliance Consequences

1. **First violation**: Warning + 30-day remediation period
2. **Second violation**: Node suspended (Karma = 0)
3. **Third violation**: Node removed from network

## Reporting Wildlife Incidents

If any wildlife interaction occurs:

1. Document with photos immediately
2. Submit incident report within 48 hours
3. Suspend node pending investigation
4. Implement corrective measures
5. Re-audit before reactivation

## Resources

- ISO 14851 Standard: https://standards.iteh.ai/catalog/standards/sist/23e41697-c2c9-4309-9880-4436d2fd649d/sist-en-iso-14851-2019
- Bird Netting Guide: https://www.birdbgone.com/blog/a-bird-netting-buyers-guide-quick-and-easy-tips-to-choose-the-right-net/
- ASCE 7 Wind Code: https://www.asce.org/publications-and-news/codes-and-standards
```
