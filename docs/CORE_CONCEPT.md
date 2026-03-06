# Windminer Core Concept

## The Problem

Urban windblown litter is a persistent environmental challenge:
- Plastic bags, wrappers, and lightweight debris travel freely through cities
- Storm drains become clogged, causing flooding during monsoon seasons
- Manual cleanup is labor-intensive and often reactive rather than preventive
- Debris crosses watershed boundaries, affecting downstream ecosystems

## The Solution: Windnets

A **windnet** is a porous mesh barrier strategically placed at locations where urban wind patterns naturally concentrate airborne debris. Think of it as a "trash collector" positioned at wind-current climax zones.

```
┌──────────────────────────────────────────────────────────────────┐
│                     CANYON AIRFLOW DIAGRAM                       │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│     Wind Direction → → → → → → → → → → → → → → → →              │
│                                                                  │
│   ┌─────────┐                                    ┌─────────┐    │
│   │ Building│                                    │ Building│    │
│   │    h    │                                    │    h    │    │
│   │         │      ╭───────────────────╮         │         │    │
│   │         │      │    VORTEX         │         │         │    │
│   │         │      │    circulation    │         │         │    │
│   │         │      │         ↓         │         │         │    │
│   │         │      │    ┌─────┐        │         │         │    │
│   │         │      │    │NET  │ ← Place│         │         │    │
│   │         │      │    │here │  here  │         │         │    │
│   │         │      │    └─────┘        │         │         │    │
│   └─────────┘      ╰───────────────────╯         └─────────┘    │
│   Windward                                    Leeward           │
│   Wall                                        Wall              │
│                                                                  │
│   Street Width = w                                               │
│   Optimal Net Height: z/h ∈ [0.3, 0.8]                          │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

## How It Works

### Step 1: Identify Canyon Geometry
Measure building height (h) and street width (w) to calculate aspect ratio h/w.
- **h/w ≥ 0.7:** Skimming flow (ideal for windnets)
- **0.3 ≤ h/w ≤ 0.7:** Wake interference
- **h/w ≤ 0.3:** Isolated roughness

### Step 2: Analyze Wind Climate
Use historical wind data to determine:
- Prevailing wind directions by season
- Effective wind time (hours with sufficient speed)
- Cross-canyon wind component: $U_\perp = U \cdot |\cos(\theta_w - \theta_c)|$

### Step 3: Locate Convergence Zones
Identify leeward corners where:
- Vortex descends along the building face
- Debris naturally accumulates
- Minimal impact on pedestrians and traffic

### Step 4: Calculate Expected Capture
$$\overline{M_{\text{cap}}} \approx C_\ell^* \, U_{\text{imp}}^* \, A_{\text{net}} \, \eta_{\text{net}} \, T_{\text{wind}}$$

### Step 5: Score Eco-Impact
$$E_{\text{windnet}} = \left(\frac{\overline{M_{\text{cap}}}}{M_{\text{ref}}}\right)(1 - R_{\text{nuisance}})$$

## Phoenix-Specific Parameters

| Parameter | Value | Source |
|-----------|-------|--------|
| Mean Wind Speed (10m) | 3.0–3.3 m/s | Sky Harbor METAR |
| Effective Wind Hours | ~2,000 h/year | Threshold U ≥ 1.5 m/s |
| Typical Canyon h/w | 1.0–2.0 | Downtown GIS |
| Net Porosity | 0.5–0.7 | Aerodynamic studies |
| Capture Efficiency | 0.3–0.5 | Field calibration |
| Reference Capture Mass | 150 kg/year | High-litter corridor |

## Expected Outcomes

- **Per Net:** 100–160 kg of litter captured annually
- **City-Wide (100 nets):** 10–16 metric tons/year reduction
- **Storm Drain Impact:** Reduced clogging during monsoon season
- **Maintenance:** Weekly collection by existing city crews
- **Eco-Impact Score:** 0.86–0.90 (high benefit, low nuisance)

## Validation Approach

1. **Desktop Phase:** GIS + wind data → candidate sites
2. **Field Phase:** Sensor deployment + litter counts → parameter calibration
3. **Iterative Refinement:** Back-solve for $C_\ell^*$ and $\eta_{\text{net}}$
4. **Scale-Up:** Apply validated model to remaining candidates
```
