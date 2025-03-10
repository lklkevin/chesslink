# ChessLink Sensor Circuit Design

This document provides technical details about the sensor circuit design for the ChessLink chess board.

## Sensor System Overview

The ChessLink uses an array of Hall effect sensors to detect the presence and position of chess pieces. Each piece contains a small magnet, allowing the sensors to detect when a piece is placed on a square.

## Hall Effect Sensor Operation

![Hall Effect Sensor Principle](../public/images/hall_effect_diagram.png)

Hall effect sensors work on the principle of the Hall effect:

1. When a magnet approaches the sensor, the magnetic field causes a measurable voltage change
2. This voltage is proportional to the strength of the magnetic field
3. By measuring this voltage, we can determine if a chess piece is present and its approximate height (useful for detecting different piece types)

## Circuit Schematic

The simplified schematic for a single sensor cell:

```
     VCC (5V)
       │
       ├─────┐
       │     │
       │     R1 (10kΩ)
       │     │
       │     ├─────┐
       │     │     │
       │     │     │
       │     │    ┌┴┐
       │     │    │ │ Hall Effect
       │     │    │ │ Sensor
       │     │    │ │ (SS49E)
       │     │    └┬┘
       │     │     │
       │     │     │
       └─────┘     │
                   │
                   ├──── Output Signal (to Multiplexer)
                   │
                   │
     GND ──────────┘
```

### Key Components

- **SS49E Hall Effect Sensor**: Linear Hall effect sensor with high sensitivity
- **10kΩ Pull-up Resistor**: Ensures stable signal and proper voltage levels
- **Multiplexer Input**: Connected to CD74HC4067 16-channel multiplexer

## Sensor Matrix Design

For the 8×8 chess board, we use a multiplexed approach to minimize I/O pins:

![Sensor Matrix](../public/images/sensor_matrix_diagram.png)

### Matrix Configuration

```
    C0  C1  C2  C3  C4  C5  C6  C7
R0  S00 S01 S02 S03 S04 S05 S06 S07
R1  S10 S11 S12 S13 S14 S15 S16 S17
R2  S20 S21 S22 S23 S24 S25 S26 S27
R3  S30 S31 S32 S33 S34 S35 S36 S37
R4  S40 S41 S42 S43 S44 S45 S46 S47
R5  S50 S51 S52 S53 S54 S55 S56 S57
R6  S60 S61 S62 S63 S64 S65 S66 S67
R7  S70 S71 S72 S73 S74 S75 S76 S77
```

Where Sxy represents the sensor at row x, column y.

## Multiplexer Configuration

We use four 16-channel multiplexers (CD74HC4067) to read from all 64 sensors:

- **Multiplexer 1**: Reads sensors S00-S17 (Rows 0-1)
- **Multiplexer 2**: Reads sensors S20-S37 (Rows 2-3)
- **Multiplexer 3**: Reads sensors S40-S57 (Rows 4-5)
- **Multiplexer 4**: Reads sensors S60-S77 (Rows 6-7)

### Multiplexer Control Lines

Four digital pins control the channel selection for all multiplexers:
- S0: Controls least significant bit of channel selection
- S1: Controls second bit of channel selection
- S2: Controls third bit of channel selection
- S3: Controls most significant bit of channel selection

Each multiplexer has its own enable line to activate only when reading from that group of sensors.

## Sensitivity Calibration

The Hall effect sensors require careful calibration to ensure reliable detection:

1. **Reference Voltage**: Established at system startup
2. **Threshold Adjustment**: Sets detection threshold based on the specific magnets used
3. **Software Filtering**: Applies digital filtering to remove noise
4. **Dynamic Calibration**: Periodically recalibrates to adjust for environmental changes

## Sensor Placement Considerations

For optimal performance:

- **Consistent Height**: Sensors must be at a consistent distance from the top surface
- **Alignment**: Each sensor must be centered under its chess square
- **Interference Mitigation**: Keep sensors away from ferromagnetic materials
- **Shielding**: Add shielding between adjacent sensors to prevent cross-talk

## Signal Processing Pipeline

The signal from the sensors undergoes the following processing:

1. **Analog Reading**: Raw voltage reading from the Hall effect sensor
2. **Multiplexing**: Channel selection and reading
3. **ADC Conversion**: Converting analog voltage to digital value
4. **Thresholding**: Determining if a piece is present based on threshold
5. **Debouncing**: Ensuring stable readings by requiring multiple consistent samples
6. **Board State Update**: Updating the digital representation of the board

## Electrical Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Supply Voltage | 5V | For Hall effect sensors |
| Current Draw (per sensor) | ~10mA | When active |
| Total Sensor Array Current | ~640mA | Maximum (all sensors) |
| Signal Voltage Range | 0-5V | Analog output |
| Detection Distance | 5-15mm | Depends on magnet strength |
| Sensitivity | 1.4mV/G | SS49E typical |
| Response Time | <20ms | For reliable detection |

## Design Considerations

### Noise Reduction

- Use twisted pair wiring for analog signals
- Include decoupling capacitors near sensors
- Implement digital filtering in firmware
- Create separate ground planes for analog and digital circuits

### Power Management

- Power sensors only when scanning
- Implement power cycling for battery operation
- Add low-dropout (LDO) regulators for clean power
- Include bulk capacitors for stable supply during peak demand

## Testing and Verification

Each sensor circuit should be tested for:

1. **Detection Range**: Verify detection at appropriate distances
2. **Consistency**: Ensure similar response across all cells
3. **Interference**: Test for cross-talk between adjacent sensors
4. **Temperature Stability**: Verify operation across temperature range
5. **Noise Rejection**: Confirm reliable operation under noisy conditions

---

*For detailed electrical schematics and PCB layout files, refer to the design files in the hardware/pcb directory.* 