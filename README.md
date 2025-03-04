# Chesslink - Interactive Smart Chessboard

This repository contains the source code and documentation for a **smart chessboard** that detects piece movements, tracks games in real time, and provides training features. It combines physical gameplay with digital benefits like move suggestions, audio feedback, and online connectivity.

---

## Table of Contents

1. [Overview](#overview)  
2. [Features](#features)  
3. [Hardware Overview](#hardware-overview)  
4. [Software Overview](#software-overview)  
5. [Installation](#installation)  
6. [Usage](#usage)  
7. [Configuration](#configuration)  
8. [Contributing](#contributing)
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
   As soon as you move a piece, the board’s sensors note the change in position.

2. **Arduino → Web App**  
   The Arduino sends the move data via serial or other communication channel to our Node.js-based server.

3. **Web App & Database**  
   - Logs moves, stores games, and delivers real-time visual updates.  
   - Offers analysis features, user accounts, and game history.

4. **Feedback to the Board**  
   - LED lights or audio prompts may be triggered based on the game state or training mode settings.

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
   - **Configure Serverless Functions:** If deploying your web app serverlessly (e.g., on Netlify, Vercel, or AWS Lambda), set up your functions and environment variables according to your platform’s guidelines.

---
## Usage

1. **Start the Development Server**  
   ```bash
   npm run dev
   ```
   **Visit [http://localhost:3000](http://localhost:3000)** (or the configured port) to view the web interface.

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