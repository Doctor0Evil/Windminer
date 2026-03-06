# Quick Start Guide

## Prerequisites

- C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 2019+)
- CMake 3.16+
- Rust 1.70+ (optional, for Rust modules)
- Git

## Installation

### Option A: C++ Only

```bash
git clone https://github.com/ecotribute/windminer.git
cd windminer
mkdir build && cd build
cmake ..
make -j4
```

### Option B: Full Stack (C++ + Rust + ALN)

```bash
git clone https://github.com/ecotribute/windminer.git
cd windminer

# Build C++ components
mkdir build && cd build
cmake .. -DBUILD_RUST=ON -DBUILD_ALN=ON
make -j4

# Build Rust components
cd ../src/rust
cargo build --release
```

## Running Your First Analysis

### Step 1: Prepare Input Data

Create a CSV file with canyon data:

```csv
canyon_id,lat,lon,h_m,w_m,theta_c_deg
PHX-C001,33.4484,-112.0740,30,20,0
PHX-C002,33.4490,-112.0750,25,18,90
PHX-C003,33.4495,-112.0760,35,22,45
```

### Step 2: Run the Planner

```bash
./windminer_plan \
  --input data/phoenix_canyons.csv \
  --wind-data data/phoenix_wind_rose.csv \
  --output results/ranked_sites.csv \
  --eco-score
```

### Step 3: Review Results

```csv
canyon_id,ecoimpactscore,expected_capture_kg_per_year,net_height_m,net_width_m,recommendation
PHX-C001,0.88,144,8,25,DEPLOY
PHX-C002,0.82,112,6,20,DEPLOY
PHX-C003,0.71,78,6,18,REVIEW
```

## Understanding the Output

| Column | Meaning |
|--------|---------|
| `ecoimpactscore` | 0–1 score balancing capture vs. nuisance |
| `expected_capture_kg_per_year` | Predicted annual litter mass |
| `net_height_m` | Recommended net vertical span |
| `net_width_m` | Recommended net horizontal span |
| `recommendation` | DEPLOY, REVIEW, or SKIP |

## Next Steps

1. **Field Validation:** Deploy sensors at top-ranked sites
2. **Parameter Calibration:** Back-solve for local $C_\ell^*$ and $\eta_{\text{net}}$
3. **Scale-Up:** Apply validated model to full city inventory
4. **Integration:** Connect to EcoNet dashboard for monitoring

## Troubleshooting

| Issue | Solution |
|-------|----------|
| CMake fails to find compiler | Install build-essential (Linux) or Xcode CLI (macOS) |
| Wind data file not found | Ensure path is relative to project root |
| Low eco-impact scores | Check nuisance penalty parameters in config |
| Rust build fails | Update to Rust 1.70+ via rustup |

## Getting Help

- Documentation: `/docs` folder
- Issues: GitHub Issues tab
- Discussion: GitHub Discussions tab
```
