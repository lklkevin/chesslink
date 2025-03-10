# ChessLink - Smart Hardware Chess Interface

![ChessLink Banner](public/images/chesslink_banner.png)

**ChessLink** is a smart chess interface that bridges physical chess with digital capabilities. The hardware-focused system uses custom sensor arrays to detect moves on a physical board and synchronize them with a digital platform for analysis, training, and connected play.

---

## Table of Contents

1. [Hardware Architecture](#hardware-architecture)
2. [System Overview](#system-overview)
3. [Features](#features)
4. [Software Components](#software-components)
5. [Development Status](#development-status)
6. [Installation](#installation)
7. [Usage](#usage)
8. [Configuration](#configuration)
9. [Contributing](#contributing)

---

## Hardware Architecture

### Core Hardware Components

- **Sensor Array**: 64-square (8×8) matrix of Hall effect sensors that detect magnetic chess pieces, 64-square (8x8) matrix of phototransistors which measure current, 64-square (8×8) matrix of RGB LEDs, 64-square (8×8) matrix of IR emitters.
- **LED System**: RGB LEDs integrated into each square for visual feedback and move guidance
- **Microcontroller Unit**: ESP32-C3 (master) combined with 16 ATtiny microcontrollers (slave) communicating through UART
- **PCB Design**: Custom multi-layer PCB (currently in development) integrating all electronic components
- **Power System**: 5V system with regulated power distribution for sensors, LEDs, and microcontrollers
- **I/O Interface**: Serial/USB connection to host application, with Bluetooth/WiFi capabilities

### PCB Development (In Progress)

The team is currently developing a custom PCB solution that will:

- **Integrate all sensor components** into a compact, reliable board design
- **Employ multiple controllers** to efficiently manage 64 squares with minimal pins and low latency
- **Include dedicated LED drivers** for controlling RGB LEDs at each board position
- **Provide noise filtering** to ensure accurate sensor readings
- **Support multiple communication protocols** including I2C, UART, and SPI
- **Feature expansion headers** for future hardware add-ons

### Sensor Technology

In prototype 1, the ChessLink system uses the following two design mechanisms for detecting pieces: 

1. Hall effect sensors placed beneath each square of the chessboard. These sensors:
- Detect the magnetic field of specially prepared chess pieces
- Offer reliable, precise detection regardless of lighting conditions
- Operate with minimal power consumption
- Provide long-term reliability without mechanical wear

2. Phototransisters placed beneath each square of the chessboard. These sensors: 
- Detect the analog light emitance reflectance of a piece based on shined light against the bottom of a piece.
   - Enables both RGB LED emittance and IR LED emmitance detection at 940nm.
- Offer cheap, piece and piece-type detection
- Operates with minimal power
- Wired to a pull-down resister to ensure that ADC voltage can be measured. 

Based on results of the prototype 1 PCB, the board will be adjusted accordingly. The board can utilize multiple sensors combined to provide accurate results. As the Hall-Effect system detects that a piece exists but does not detect which piece the two can be utilized together to create a reliable system of detection.

### Physical Dimensions

| Component | Specification |
|-----------|---------------|
| Board Size | Standard tournament size: 50cm × 50cm |
| Square Size | 5.0cm × 5.0cm |
| Sensor Spacing | 6.25cm center-to-center |
| Board Thickness | 2.5cm including electronics |
| Chess Piece Base | 3.8cm diameter with embedded magnets |

### Circuit Design

The sensor array is organized in an 8×8 matrix configuration:

```
+-----------------+    +-----------------+    +-----------------+
| Sensor Module   |    | ATtiny1616 (x16) |    | ESP32-C3        |
| Phototransistor|====> Each Handles 2×2 |====> Main Controller |
| Hall Effect     |    | Grid Section     |    | (UART Master)   |
| IR Emitter      |    | UART to ESP32    |    |                 |
| RGB LEDs        |    | (4 ATtiny in P1) |    |                 |
+-----------------+    +-----------------+    +-----------------+
                                                             
                                               | USB to PC (Debugging)
                                               v
                                        +-----------------+
                                        | CP2102          |
                                        | USB-to-Serial   |
                                        | Converter       |
                                        +-----------------+
```

Prototype 1: 4 ATtiny1616 -> Covers 16 squares (4×4)

Final Design: 16 ATtiny1616 -> Covers 64 squares (8×8)


---

## System Overview

ChessLink creates a seamless connection between physical chess play and digital analysis by:

1. **Detecting physical moves** through the sensor array
2. **Validating moves** against chess rules
3. **Providing feedback** through LEDs and sound
4. **Synchronizing** with the web application
5. **Enabling analysis** and training features

The system is designed for:
- Chess enthusiasts wanting physical play with digital benefits
- Beginners learning through interactive guidance
- Events and streamers needing real-time digital tracking

---

## Features

- **Real-Time Move Detection**: Automatically captures piece movements and positions
- **Visual Guidance**: LED system highlights valid moves, checks, and training suggestions
- **Audio Feedback**: Sound effects and spoken move announcements
- **Training Mode**: Receive move suggestions and error notifications
- **Analysis Integration**: Connect with chess engines for post-game analysis
- **Game Recording**: Automatically stores game history for review
- **Accessibility Features**: Audio announcements and high-contrast visual cues

---

## Software Components

The software stack complements the hardware system:

1. **Firmware**: ESP32/Arduino code handling sensor data and LED control
2. **Web Application**: React-based interface visualizing the board and providing analysis
3. **API Layer**: Connects hardware with software systems
4. **Optional Server**: For online play and advanced features

See the [software documentation](app/README.md) for detailed information.

---

## Development Status

**Current Status: Hardware Prototype Phase**

- ✅ Hardware architecture designed
- ✅ Sensor and LED testing completed
- ✅ Software interface working
- 🔄 PCB design in progress
- 🔄 Final component selection being evaluated
- 🔄 4×4 prototype board operational
- ⬜ Full 8×8 board assembly
- ⬜ Case design and manufacturing

---

## Installation

1. **Clone the Repository**  
   ```bash
   git clone https://github.com/lklkevin/chesslink.git
   cd chesslink
   ```

2. **Install Software Dependencies**  
   ```bash
   npm install
   ```

3. **Hardware Setup (for developers)**
   - Upload the appropriate firmware to your ESP32/Arduino using the Arduino IDE
   - Connect the prototype board via USB
   - See [hardware setup guide](hardware/SETUP.md) for detailed instructions

---

## Usage

1. **Start the Application**  
   ```bash
   npm run dev
   ```
   Access the interface at [http://localhost:8080](http://localhost:8080)

2. **Connect Hardware** (if available)
   - Connect the ChessLink board via USB
   - The app will automatically detect and connect to the board

3. **Available Modes**
   - **Digital-only mode**: Play on screen without physical board
   - **Connected mode**: Synchronize physical and digital boards
   - **Training mode**: Receive move suggestions and feedback
   - **Analysis mode**: Review games with engine analysis

---

## Configuration

- **Hardware Configuration**: See `hardware/firmware/config.h` for sensor settings
- **Software Environment Variables**: Create a `.env` file for API keys and database connections
- **Sound Settings**: Configure audio feedback at [/sounds](http://localhost:8080/sounds)

---

## Contributing

We welcome contributions, especially in hardware design and firmware development!

1. **Fork the Repository**  
2. **Create a Feature Branch**: `git checkout -b feature/amazing-feature`  
3. **Commit Your Changes**: `git commit -m 'Add amazing feature'`  
4. **Push to Branch**: `git push origin feature/amazing-feature`  
5. **Open a Pull Request**

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

---

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE) for details.

---

*Documentation last updated: August 2023*
