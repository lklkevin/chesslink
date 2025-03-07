# Chesslink - Interactive Smart Chessboard

This repository contains the source code and documentation for a **smart chessboard** that detects piece movements, tracks games in real time, and provides training features. It combines physical gameplay with digital benefits like move suggestions, audio feedback, and online connectivity.

---

## Table of Contents

1. [Overview](#overview)  
2. [Features](#features)  
3. [Hardware Overview](#hardware-overview)  
4. [Software Overview](#software-overview)  
5. [System Architecture](#system-architecture)  
6. [Installation](#installation)  
7. [Usage](#usage)  
8. [Configuration](#configuration)  
9. [Contributing](#contributing)
---

## Overview

**What problem it solves:**  
- Preserves the **physical** feel of playing chess while adding **digital** capabilities like move recording and analysis.  
- Assists beginners in learning with real-time feedback and highlighted valid moves.  
- Enables streamlined post-game analysis for players who want to review their decisions and improve.

**Who it is for:**  
- Chess enthusiasts seeking a hybrid of real-board and online analysis.  
- Beginners who learn best by doing and receiving immediate feedback.  
- Organized events and streamers who need real-time, interactive move tracking.

**Key insights:**  
- All piece movements are captured automatically, no more manual recording of games.  
- Integrates with a **web application** to store and analyze moves using a chess engine or other learning tools.

---

## Features

- **Real-Time Move Tracking**  
  Detects piece movements via sensors and updates a digital interface.

- **LED Feedback**  
  Highlights valid moves, checks, or other important chess events.

- **Audio Prompts**  
  Announces moves (e.g., in algebraic notation) and signals special situations like check or checkmate.

- **Online Integration**  
  Syncs with a web application to record games, provide analysis, or connect to remote opponents.

- **Training & Analysis Modes**  
  - **Training**: Offers real-time suggestions and visual/audio guidance.  
  - **Analysis**: Provides post-game data and best-move recommendations.

---

## Hardware Overview

- **Sensors**: Hall-effect sensors, near-IR sensors, or photodiodes to detect when pieces are moved or lifted.  
- **Arduino Nano (or similar)**: Reads sensor data and sends updates via serial communication.  
- **LEDs**: Provide guidance for moves or highlight errors and special scenarios.  
- **Board Size**: Prototype is 4x4 for proof of concept; the ultimate goal is a full 8x8 board.

This approach preserves the physical experience of chess while adding an intelligent, interactive layer.

---

## Software Overview

1. **Board Sensors → Arduino**  
   As soon as you move a piece, the board's sensors note the change in position.

2. **Arduino → Web App**  
   The Arduino sends the move data via serial or other communication channel to our Node.js-based server.

3. **Web App & Database**  
   - Logs moves, stores games, and delivers real-time visual updates.  
   - Offers analysis features, user accounts, and game history.

4. **Feedback to the Board**  
   - LED lights or audio prompts may be triggered based on the game state or training mode settings.

---

## System Architecture

The ChessLink system is designed with a layered architecture that connects physical chess pieces to a digital platform through several interconnected components:

### 1. Physical Layer

#### Chessboard & Sensor Array
- **Hall Effect Sensors**: Each square contains an IR sensor with an RGB LED to detect piece presence and provide visual feedback
- **Digital I/O**: 1 digital pin per sensor module to read the sensor state
- **Sensor Arrangement**: 8×8 grid (64 squares) with consistent 3cm spacing between sensors
- **Power Requirements**: 5V power supply for sensors and microcontroller components

#### Microcontroller System
- **ESP32-S3 Microcontroller**: 
  - Central processing unit running at 160MHz
  - Handles sensor data collection, board state determination, and communication
  - Runs frequent scanning cycles (every second) to detect changes in piece positions
  - Converts raw sensor readings into a digital representation of the chessboard
  - Processes the data to determine legal moves and board state
  - Translates the board state into FEN notation for software integration

- **Arduino Nano**: 
  - Secondary microcontroller acting as a co-processor when needed
  - Connected to the ESP32-S3 through I2C interface
  - Assists with sensor reading and processing tasks

- **Connectivity Options**:
  - **WiFi Module**: For wireless connectivity to the network
  - **Bluetooth (BLE) 5**: For direct connection to mobile devices or computers
  - **TCP Socket/WebSocket/Serial Communication**: Various protocol options to connect with the software application

### 2. Software Layer

#### Local Software Application
- **Localized Board State Management**:
  - Maintains the current state of the chessboard
  - Tracks the history of moves and past positions
  - Validates moves based on chess rules
  - Updates the FEN (Forsyth–Edwards Notation) representation
  - Performs regular checks for board state validity

- **User Interface Components**:
  - Sends voice prompts when pieces are moved
  - Sends LED signals to indicate where moves are legal
  - Highlights squares for move suggestions
  - Provides audio feedback on game state

- **Local Game Visualization**:
  - Web-based interface or desktop application
  - Renders the current board visually
  - Displays game information and history
  - Python app for more advanced functionality

#### Server-Side Architecture (Optional)

- **Backend Server**:
  - Built with Python or Node.js
  - Accepts JSON data regardless of transmission method
  - Handles API requests
  - Stores history of all games played
  - Updates games for web app viewing
  - Allows for multiple games on multiple servers to be played simultaneously

- **Database System**:
  - Stores game history, user profiles, and analysis data
  - Provides persistent storage across sessions
  - Enables game replay and analysis features

#### Frontend Server
- **Web Application**:
  - Built with Node.js and React
  - Responsive design with Tailwind CSS
  - Real-time updates of ongoing games
  - Analysis tools and interfaces

### 3. Communication Pathways

#### Hardware to Software Communication
- **Processor to App**: 
  - Data flow from the microcontroller system to the local software
  - Uses TCP socket, mounted files, or serial connection depending on setup

#### App to Processor Communication
- **Software commands to hardware**:
  - Control signals for LED feedback
  - Request for board state updates
  - Training mode instructions

#### Local to Server Communication
- **API/JSON Data Exchange**:
  - Secure transmission of game data
  - Authentication for user-specific features
  - Real-time updates through WebSocket connections

### 4. System Integration

The entire system follows a layered approach where each component has a specific responsibility:

1. **Sensing Layer**: Detects the physical position of chess pieces
2. **Processing Layer**: Converts sensor data into chess notation and game state
3. **Application Layer**: Provides user interface and game management features
4. **Optional Server Layer**: Enables online features, persistent storage, and multiplayer capabilities

This architecture allows for flexibility in deployment - the system can work as a standalone device with the local application, or as a connected device with full server integration for advanced features.

### 5. System Architecture Diagram

```
+--------------------------------------------------------------------------------------------+
|                                CHESSLINK SYSTEM ARCHITECTURE                               |
+--------------------------------------------------------------------------------------------+

+------------------------+        +------------------------+        +------------------------+
|   PHYSICAL LAYER       |        |   PROCESSING LAYER     |        |   APPLICATION LAYER    |
|                        |        |                        |        |                        |
|  +------------------+  |        |  +------------------+  |        |  +------------------+  |
|  | Chess Board      |  |        |  | ESP32-S3         |  |        |  | Local Software   |  |
|  |                  |  |        |  | Microcontroller  |  |        |  | Application      |  |
|  | +-------+        |  |        |  |                  |  |        |  |                  |  |
|  | | IR    |  x64   |  |        |  | +-------------+  |  |        |  | +-------------+  |  |
|  | | Sensor|        |  |        |  | | Sensor Data |  |  |        |  | | Board State |  |  |
|  | | + LED |        |  |        |  | | Processing  |  |  |        |  | | Management  |  |  |
|  | +-------+        |  |        |  | +-------------+  |  |        |  | +-------------+  |  |
|  |                  |  |        |  |                  |  |        |  |                  |  |
|  | +-----------+    |  |        |  | +-------------+  |  |        |  | +-------------+  |  |
|  | | Chess     |    |  |        |  | | FEN         |  |  |        |  | | Game UI &   |  |  |
|  | | Pieces    |    |  |        |  | | Conversion  |  |  |        |  | | Visuals     |  |  |
|  | +-----------+    |  |        |  | +-------------+  |  |        |  | +-------------+  |  |
|  +------------------+  |        |  |                  |  |        |  |                  |  |
|          |             |        |  | +-------------+  |  |        |  | +-------------+  |  |
|  +------------------+  |        |  | | Move        |  |  |        |  | | Move        |  |  |
|  | Arduino Nano     |<------------>| | Validation  |  |  |        |  | | History &   |  |  |
|  | (Co-processor)   |  |        |  | +-------------+  |  |        |  | | Analysis    |  |  |
|  +------------------+  |        |  +------------------+  |        |  +------------------+  |
+------------------------+        +------------------------+        +------------------------+
           ^                                 ^                                 ^
           |                                 |                                 |
           |                                 |                                 |
           v                                 v                                 v
+--------------------------------------------------------------------------------------------+
|                                   COMMUNICATION LAYER                                      |
|                                                                                            |
|  +-------------+                  +----------------+               +--------------------+  |
|  | I2C         |                  | TCP/Serial/    |               | Web API/WebSocket  |  |
|  | Interface   |<---------------->| Bluetooth      |<------------->| Communication      |  |
|  +-------------+                  +----------------+               +--------------------+  |
+--------------------------------------------------------------------------------------------+
           ^                                                             ^
           |                                                             |
           v                                                             v
+------------------------+                                    +------------------------+
|   OPTIONAL SERVER      |                                    |   EXTERNAL SYSTEMS     |
|   LAYER                |                                    |                        |
|                        |                                    |  +------------------+  |
|  +------------------+  |                                    |  | Chess Engines    |  |
|  | Backend Server   |  |                                    |  |                  |  |
|  | (Node.js/Python) |  |                                    |  | +-------------+  |  |
|  |                  |  |                                    |  | | Move        |  |  |
|  | +-------------+  |  |                                    |  | | Analysis    |  |  |
|  | | Game        |  |  |                                    |  | +-------------+  |  |
|  | | Storage     |  |  |                                    |  +------------------+  |
|  | +-------------+  |  |                                    |                        |
|  |                  |  |                                    |  +------------------+  |
|  | +-------------+  |  |                                    |  | Online Chess     |  |
|  | | User        |  |  |                                    |  | Platforms        |  |
|  | | Management  |  |  |                                    |  +------------------+  |
|  | +-------------+  |  |                                    |                        |
|  +------------------+  |                                    +------------------------+
|                        |
|  +------------------+  |                DATA FLOW DIRECTION
|  | Database         |  |                ------------------>
|  |                  |  |                                 
|  | +-------------+  |  |                CHESS MOVES FLOW:
|  | | Game History|  |  |                1. Physical piece movement detected by sensors
|  | +-------------+  |  |                2. Raw sensor data processed by microcontrollers
|  |                  |  |                3. Board state converted to FEN notation
|  | +-------------+  |  |                4. Move validated against chess rules
|  | | User Data   |  |  |                5. Valid moves sent to software application
|  | +-------------+  |  |                6. Game state updated and visualized
|  +------------------+  |                7. Optional server storage and analysis
+------------------------+                8. Optional feedback sent back to board (LEDs)
```

---

## Installation

1. **Clone the Repository**  
   ```bash
   git clone https://github.com/lklkevin/chesslink.git
   cd chesslink

2. **Install Dependencies**  
   Make sure you have [Node.js](https://nodejs.org/) installed, then run:

   ```bash
   npm install
   ```

   This command installs all the Node.js packages listed in `package.json` that are required for the application.

3. **Arduino Setup (Optional)**  
   - **Install the Arduino IDE:** Download from [here](https://www.arduino.cc/en/software).  
   - **Upload Firmware:** Connect your Arduino Nano (or compatible board) and upload the sketch from the `firmware` folder.

4. **Serverless/Hosting Setup (Optional)**  
   - **Configure Serverless Functions:** If deploying your web app serverlessly (e.g., on Netlify, Vercel, or AWS Lambda), set up your functions and environment variables according to your platform's guidelines.

---
## Usage

1. **Start the Development Server**  
   ```bash
   npm run dev
   ```
   **Visit [http://localhost:8080](http://localhost:8080)** to view the web interface.

2. **Connect the Chessboard**  
   - **Power On the Board:** Ensure your Arduino-based chessboard is powered and connected.  
   - **Real-Time Updates:** As you move pieces, the sensor data is transmitted to the web application, reflecting live game updates.

3. **Training Mode**  
   - **Enable Training Mode:** Receive LED and audio prompts guiding you with valid moves or alerting you to potential mistakes.

4. **Analysis Mode**  
   - **Access Saved Games:** View move-by-move breakdowns and receive recommendations from a chess engine or analysis library.

5. **Build for Production**  
   ```bash
   npm run build
   ```
   **Deploy the built output along with your serverless functions to your chosen hosting platform.**

---

## Configuration

- **Environment Variables:**  
  Create a `.env` file (or set variables in your hosting platform). Examples include:
  - `PORT`
  - Database connection strings
  - API keys for chess engines, etc.

- **Arduino Firmware:**  
  Adjust sensor thresholds, LED brightness, or communication protocols in the `.ino` file as needed.

- **Web Application Settings:**  
  Tweak configuration files (e.g., `config.js`) to adjust UI settings, default modes, or integrate external services.

---

## Contributing

We welcome pull requests, bug reports, and feature suggestions! To contribute:

1. **Fork the Repository**  
2. **Create a New Feature Branch** from the `main` branch.  
3. **Commit and Push Your Changes** with clear commit messages.  
4. **Open a Pull Request** on GitHub, describing your changes and linking any relevant issues.

*For major changes, consider opening an issue first to discuss your proposed improvements.*
