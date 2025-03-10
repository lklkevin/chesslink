# ChessLink Hardware Setup Guide

This guide provides detailed instructions for setting up and connecting the ChessLink hardware components.

## Overview

The ChessLink hardware system consists of:
- Sensor matrix for detecting chess pieces
- LED system for visual feedback
- Microcontroller system for processing
- Power management circuitry
- Communication interfaces

![Hardware Design](../public/images/hardware_diagram.png)

## Current Development Status

**Note: The hardware is currently in the prototype phase.** 

- 4×4 prototype board is operational
- Full PCB design is in progress
- Component selection is being finalized

## Components List

### Core Components

| Component | Quantity | Purpose |
|-----------|----------|---------|
| ESP32-C3 | 1 | Main microcontroller |
| Arduino Nano | 1 | Co-processor (optional) |
| Hall Effect Sensors (SS49E) | 64 | Chess piece detection |
| RGB LEDs (WS2812B) | 64 | Square illumination |
| CD74HC4067 Multiplexers | 4 | Sensor matrix management |
| TLC5940 LED Drivers | 2 | LED control |
| 5V Voltage Regulator | 1 | Power management |
| Custom PCB | 1 | Component integration |

### Additional Components

| Component | Quantity | Purpose |
|-----------|----------|---------|
| USB-C Port | 1 | Power and data connection |
| Level Shifters | 4 | Signal voltage conversion |
| Bypass Capacitors | 20-30 | Power stabilization |
| Chess Pieces with Magnets | 32 | Magnetic detection |
| 3D Printed Case | 1 | Housing components |

## PCB Design Specifications

The PCB is designed to fit inside a standard chess board:

- **Dimensions**: 40cm × 40cm (main board)
- **Layers**: 4-layer design
  - Top layer: Components and signal routing
  - Inner layer 1: Ground plane
  - Inner layer 2: Power plane
  - Bottom layer: Additional routing and components
- **Power Requirements**: 5V DC, 2A minimum
- **Mounting**: M3 mounting holes at corners and center

**PCB Design Notes:**
- Separate analog and digital grounds
- Power filtering capacitors near all ICs
- Sensor traces kept short to minimize noise
- High current traces for LED power distribution (min. 30mil width)

## Wiring Diagram

### Sensor Matrix Connection

```
Hall Sensor Matrix   →   Multiplexers   →   ESP32-C3
(8 rows × 8 columns)     (4× CD74HC4067)    (GPIO pins)

Row Select Lines:
- S0: GPIO 4
- S1: GPIO 5
- S2: GPIO 6
- S3: GPIO 7

Multiplexer Enable:
- EN1: GPIO 8 (columns 0-15)
- EN2: GPIO 9 (columns 16-31)
- EN3: GPIO 10 (columns 32-47)
- EN4: GPIO 11 (columns 48-63)

Analog Inputs:
- Input 1: GPIO 12 (ADC channel 1)
- Input 2: GPIO 13 (ADC channel 2)
- Input 3: GPIO 14 (ADC channel 3)
- Input 4: GPIO 15 (ADC channel 4)
```

### LED Control Connection

```
LED Matrix         →   LED Drivers    →   ESP32-C3
(8 rows × 8 columns)   (2× TLC5940)      (GPIO pins)

- Data: GPIO 18
- Clock: GPIO 19
- Latch: GPIO 20
- Blank: GPIO 21
```

## Assembly Instructions

### Current 4×4 Prototype

1. **Sensor Preparation**
   - Solder sensors to perfboard with proper spacing
   - Connect common ground
   - Wire sensors to multiplexer inputs

2. **Microcontroller Connection**
   - Connect ESP32-C3 to multiplexers as per wiring diagram
   - Connect ESP32-C3 to LED drivers
   - Connect power lines (VCC and GND)

3. **Power Connection**
   - Connect 5V power supply to the system
   - Add decoupling capacitors near ICs

4. **Testing the Prototype**
   - Upload test firmware
   - Verify sensor readings for each position
   - Test LED functionality for each square

### Future Full PCB Assembly

Detailed assembly instructions will be provided when the PCB design is finalized. The process will involve:

1. PCB fabrication and component sourcing
2. Surface-mount component soldering (or assembly service)
3. Through-hole component installation
4. Sensor array alignment and installation
5. Chess piece preparation (magnet installation)
6. Case assembly and mounting

## Firmware Upload

1. Install Arduino IDE
2. Install ESP32 board support
3. Clone the firmware repository
4. Open `chesslink_firmware.ino`
5. Select appropriate board and port
6. Upload the firmware
7. Verify operation using Serial Monitor

## Troubleshooting

### Common Issues

1. **Sensor Detection Problems**
   - Check sensor orientation (Hall effect sensors are directional)
   - Verify multiplexer connections
   - Ensure magnets in chess pieces are correctly oriented

2. **LED Issues**
   - Check power connections (LEDs require sufficient current)
   - Verify data line connections
   - Test LED drivers with simple patterns

3. **Communication Problems**
   - Check USB cable connection
   - Verify serial port settings
   - Ensure correct firmware is loaded

## Future Hardware Improvements

- Wireless communication module
- Battery power option
- Expanded sensor types for greater reliability
- Modular design for easier maintenance

## Resources and References

- [ESP32-C3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
- [Hall Effect Sensor Specifications](https://www.honeywell.com/us/en/products/sensors)
- [LED Driver Documentation](https://www.ti.com/product/TLC5940)
- [PCB Design Guidelines](hardware/pcb/DESIGN_GUIDELINES.md)

---

*For technical assistance or to contribute to hardware development, contact the hardware team at [hardware@chesslink.org](mailto:hardware@chesslink.org)* 