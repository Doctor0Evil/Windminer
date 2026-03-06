# Architecture Overview

## System Components

```
┌─────────────────────────────────────────────────────────────────────┐
│                         WINDMINER ARCHITECTURE                      │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐             │
│  │   Data      │    │   Core      │    │   Output    │             │
│  │   Layer     │    │   Layer     │    │   Layer     │             │
│  ├─────────────┤    ├─────────────┤    ├─────────────┤             │
│  │ Wind Roses  │───▶│ Geometry    │───▶│ Ranked      │             │
│  │ Canyon GIS  │    │ Engine      │    │ Sites CSV   │             │
│  │ Building    │    │ Capture     │    │ Eco-Scores  │             │
│  │ Footprints  │    │ Model       │    │ Shard Files │             │
│  └─────────────┘    └─────────────┘    └─────────────┘             │
│         │                  │                  │                     │
│         ▼                  ▼                  ▼                     │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐             │
│  │  qpudata-   │    │  C++/Rust   │    │  ALN-       │             │
│  │  shards/    │    │  Libraries  │    │  Blockchain │             │
│  └─────────────┘    └─────────────┘    └─────────────┘             │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

## Module Breakdown

### Data Layer

| Module | Purpose | Format |
|--------|---------|--------|
| Wind Climate | Phoenix wind rose statistics | CSV |
| Canyon Geometry | Building height, street width, orientation | CSV/GeoJSON |
| qpudatashards | Standardized output records | CSV (RFC-4180) |

### Core Layer

| Module | Language | Purpose |
|--------|----------|---------|
| WindnetGeometry | C++ | Canyon classification, aspect ratio |
| WindClimatePhoenix | C++ | Wind data queries, seasonal U⊥ |
| WindnetCaptureModel | C++/Rust | Capture equation implementation |
| WindnetEcoImpact | C++ | Eco-score calculation |
| ShardIo | C++ | CSV read/write for shards |

### Output Layer

| Module | Purpose | Integration |
|--------|---------|-------------|
| Ranked Sites | Prioritized deployment list | City planning |
| Eco-Scores | Normalized impact metrics | EcoNet dashboard |
| ALN Records | Blockchain-verified captures | Governance tracking |

## Data Flow

```
1. INPUT: phoenix_canyons.csv + phoenix_wind_rose.csv
              │
              ▼
2. PROCESS: WindnetGeometry classifies flow regime
              │
              ▼
3. PROCESS: WindClimatePhoenix computes U⊥ and T_wind
              │
              ▼
4. PROCESS: WindnetCaptureModel calculates M_cap
              │
              ▼
5. PROCESS: WindnetEcoImpact computes E_windnet
              │
              ▼
6. OUTPUT: windnet_phoenix_prototype2026v1.csv (qpudatashard)
              │
              ▼
7. OPTIONAL: ALN blockchain record for verified captures
```

## Language Choices

| Component | Language | Rationale |
|-----------|----------|-----------|
| Core Engine | C++ | Performance, existing EcoNet compatibility |
| CLI Tools | C++ | Native integration with core |
| Sensor Firmware | Rust | Memory safety, embedded suitability |
| Blockchain Records | ALN | Governance, audit trail, Karma tracking |
| Scripts | Python/Bash | Automation, data preprocessing |

## Configuration Hierarchy

```
config/
├── default.yaml           # Base configuration
├── phoenix.yaml           # City-specific overrides
├── validation.yaml        # Field calibration settings
└── deployment.yaml        # Production parameters
```

## Extension Points

- **New Cities:** Add wind rose + canyon geometry data
- **New Metrics:** Extend EcoImpact scoring formula
- **New Sensors:** Integrate via Rust firmware module
- **New Blockchains:** ALN adapter pattern for other chains
```
