# PCB Design Guidelines for ChessLink

This document outlines the specifications and best practices for designing the ChessLink PCB.

## Design Goals

The ChessLink PCB design aims to:
1. Integrate sensors, LEDs, and control circuitry in a compact form factor
2. Ensure reliable detection of chess pieces
3. Provide bright, clear LED indicators
4. Maintain signal integrity across the board
5. Minimize power consumption
6. Facilitate easy assembly and maintenance

## PCB Specifications

### Physical Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Board Dimensions | 400mm × 400mm | Main board size |
| Board Thickness | 1.6mm | Standard thickness |
| Copper Weight | 1oz (outer layers)<br>0.5oz (inner layers) | Increased copper for power traces |
| Minimum Trace Width | 0.2mm (8mil) | For signal traces |
| Power Trace Width | 0.75mm (30mil) | For VCC and GND |
| Minimum Spacing | 0.2mm (8mil) | Between traces |
| Silkscreen | White | Component side |
| Solder Mask | Black | Both sides |
| Surface Finish | ENIG | For reliable SMT assembly |

### Layer Stack

The PCB uses a 4-layer design:
1. **Top Layer**: Components and signal routing
2. **Inner Layer 1**: Ground plane
3. **Inner Layer 2**: Power plane (segmented for different voltage domains)
4. **Bottom Layer**: Additional components and routing

## Component Placement Guidelines

### Sensor Array

- Arrange Hall effect sensors in an 8×8 grid
- Center each sensor under the corresponding chess square
- Maintain exact 50mm spacing between sensor centers
- Orient all sensors consistently (direction of sensitivity matters)

### LED Placement

- Position RGB LEDs at the center of each chess square
- Ensure consistent orientation (note WS2812B direction)
- Place LEDs on the top side of the PCB
- Add current-limiting resistors near each LED

### Microcontroller Placement

- Position the ESP32-C3 in a corner of the board to minimize interference
- Keep analog sections away from high-frequency digital signals
- Orient the MCU for easiest routing to sensors and LEDs
- Allow space for heat dissipation

### Power Circuit

- Place voltage regulator near the power input
- Add bulk capacitors (100-220μF) near the regulator
- Include filtering capacitors (0.1μF) for each IC
- Add reverse polarity protection
- Include power indicator LED

## Routing Guidelines

### General Routing

- Route critical signals first
- Use 45-degree angles (avoid 90-degree corners)
- Keep traces as short as possible
- Avoid routing sensitive signals near high-frequency lines
- Place test points at key circuit nodes

### Sensor Routing

- Use a star configuration for sensor power distribution
- Keep analog signal traces short and direct
- Route sensor signals away from LED data lines
- Add ground plane cutouts under sensitive analog traces
- Consider using shielded traces for critical sensors

### LED Data Lines

- Keep LED data lines short and direct
- Maintain consistent impedance for LED data lines
- Add series resistors at LED data inputs
- Route LED power separately from sensor power
- Follow manufacturer's recommendations for WS2812B routing

### Power Distribution

- Use power planes for main distribution
- Place decoupling capacitors near each IC power pin
- Create separate analog and digital power domains
- Include ferrite beads between power domains

## Design Files Organization

Maintain the following file structure for PCB design:
```
hardware/pcb/
├── design_files/              # KiCad or other CAD files
│   ├── chesslink_main.kicad_pro
│   ├── chesslink_main.kicad_pcb
│   └── chesslink_main.kicad_sch
├── libraries/                 # Custom component libraries
│   ├── chesslink_components.lib
│   └── chesslink_footprints.pretty/
├── manufacturing/             # Files for PCB fabrication
│   ├── gerber/
│   ├── drill/
│   └── bom.csv
├── datasheets/                # Component documentation
└── simulation/                # Circuit simulation files
```

## Design Review Checklist

Before submitting the PCB for fabrication, verify:

- [ ] All components have footprints and values assigned
- [ ] Thermal relief connections on power planes
- [ ] Adequate clearance around mounting holes
- [ ] Design rule check (DRC) passes without errors
- [ ] Sensor positions align with chess squares
- [ ] Power traces sized appropriately for current
- [ ] Test points available for key signals
- [ ] Silkscreen labels for major components
- [ ] Version number and date on silkscreen
- [ ] All nets are properly connected

## Manufacturing Considerations

- Use standard PCB materials (FR4)
- Specify black solder mask to match chessboard aesthetic
- Consider panel design for multiple boards
- Include fiducial markers for automated assembly
- Add breakaway tabs or mouse bites if using panels
- Include a unique serial number on each board

## Assembly Notes

- Consider using solder paste stencil for SMT components
- Document assembly sequence
- Mark component orientation on silkscreen
- Include polarity indicators for diodes, ICs, etc.
- Create clear assembly drawings for production

## Testing Procedures

After PCB assembly, follow these testing steps:

1. Visual inspection of all components
2. Continuity testing of power rails
3. Power-on test (without components, check for shorts)
4. Programming and basic function test
5. Sensor calibration and verification
6. LED function test
7. Full system integration test

## Revision Control

- Include version number in PCB silkscreen
- Document all design changes between revisions
- Maintain a revision history in the repository
- Create tags for each production version
- Follow semantic versioning for PCB revisions

---

*Contact the hardware team lead before making significant design changes to the PCB layout.* 