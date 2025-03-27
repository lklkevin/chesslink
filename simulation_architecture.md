# ChessLink Simulation Architecture

This document describes the architecture of the ChessLink simulation system, which allows for testing and development of the ChessLink application without requiring physical hardware.

## System Overview

The ChessLink system consists of several components that work together to visualize chess games:

1. **PGN Files**: Source of chess games in Portable Game Notation format
2. **Emulation Layer**: Two distinct emulators that process PGN files
   - **WebSocket Emulator**: Sends chess positions to the web application
   - **Serial Emulator**: Sends chess positions through a serial port
3. **Web Application**: React-based frontend that displays the chess board
4. **Hardware**: Physical chess board that connects via serial port (optional)

## Architecture Diagram

```mermaid
graph TB
    subgraph "Data Source"
        PGN[PGN Files]
    end

    subgraph "Emulation Layer"
        WSE[WebSocket Emulator]
        SE[Serial Emulator]
    end

    subgraph "Client Layer"
        WA[Web Application]
        HW[Hardware Interface]
    end

    subgraph "Presentation Layer"
        CB[Chess Board Display]
    end

    PGN --> WSE
    PGN --> SE
    WSE --> WA
    SE --> HW
    WA --> CB
    HW --> CB

    classDef primary fill:#f9f,stroke:#333,stroke-width:2px
    classDef secondary fill:#bbf,stroke:#333,stroke-width:1px
    classDef tertiary fill:#bfb,stroke:#333,stroke-width:1px
    
    class PGN primary
    class WSE,SE secondary
    class WA,HW tertiary
    class CB primary
```

## Component Details

### PGN Files
- Located in `hardware/sim/pgn/`
- Contains famous chess games in standard PGN format
- Examples include Anderssen's Immortal Game, Kasparov vs Topalov, etc.
- Serves as the source of chess positions for simulation

### WebSocket Emulator (`pgn_websocket_emulator.py`)
- Reads PGN files and converts them to FEN positions
- Runs a WebSocket server on port 8765
- Streams chess positions to web clients
- Supports game selection via query parameters
- Features loop mode for continuous playback

### Serial Emulator (`pgn_serial_emulator.py`)
- Reads PGN files and converts them to FEN positions
- Sends FEN strings through a serial port
- Can run in simulation mode without hardware
- Supports interactive port selection
- Features loop mode for continuous playback

### Web Application (`Demo.tsx`)
- React-based frontend component
- Connects to WebSocket emulator to receive positions
- Renders chess board based on FEN strings
- Provides UI for selecting games and connection modes
- Displays player indicators and game information

## Communication Flow

```mermaid
sequenceDiagram
    participant PGN as PGN Files
    participant WS as WebSocket Emulator
    participant WA as Web Application
    participant CB as Chess Board
    
    WA->>WS: Connect(game=immortal)
    activate WS
    WS->>PGN: Load(immortal.pgn)
    PGN-->>WS: Game Data
    WS-->>WA: Game Info & Available Games
    loop For each position
        WS-->>WA: FEN Position
        WA->>CB: Update Board
        Note over WS: Wait (delay seconds)
    end
    deactivate WS
```

## Serial Communication Flow

```mermaid
sequenceDiagram
    participant PGN as PGN Files
    participant SE as Serial Emulator
    participant HW as Hardware
    participant CB as Chess Board
    
    Note over SE: Start(--game=opera)
    SE->>PGN: Load(opera.pgn)
    PGN-->>SE: Game Data
    loop For each position
        SE-->>HW: FEN Position
        HW->>CB: Update Board
        Note over SE: Wait (delay seconds)
    end
```

## Simulation Modes

The ChessLink system supports multiple simulation modes to accommodate different testing scenarios:

### WebSocket Mode (for Web Testing)
```mermaid
flowchart LR
    PGN[PGN Files] --> WSE[WebSocket Emulator]
    WSE -- "ws://localhost:8765" --> WA[Web App]
    WA --> CB[Chess Board Display]
```

### Serial Mode (for Hardware Testing)
```mermaid
flowchart LR
    PGN[PGN Files] --> SE[Serial Emulator]
    SE -- "/dev/cu.usbserial" --> HW[Hardware]
    HW --> CB[Physical Board]
```

### Simulation Mode (No Hardware)
```mermaid
flowchart LR
    PGN[PGN Files] --> SE[Serial Emulator]
    SE -- Terminal Output --> DEV[Developer]
```

## Usage Example

### Running WebSocket Emulator
```bash
./hardware/sim/start_pgn_websocket.sh --game immortal
```

The script will:
1. Load the "immortal" PGN file
2. Start a WebSocket server on port 8765
3. Stream positions from the game with a delay

### Running Serial Emulator
```bash
./hardware/sim/start_pgn_serial.sh --port /dev/cu.usbserial --game kasparov_topalov
```

The script will:
1. Load the "kasparov_topalov" PGN file
2. Connect to the specified serial port
3. Stream positions from the game with a delay

## Data Flow

```mermaid
graph TD
    subgraph "Data Processing"
        PGN[PGN File] --> |Read file| PARSER[PGN Parser]
        PARSER --> |Extract moves| BOARD[Chess Board]
        BOARD --> |Generate positions| FEN[FEN Strings]
    end

    subgraph "Distribution"
        FEN --> WSS[WebSocket Server]
        FEN --> SP[Serial Port]
    end

    subgraph "Consumption"
        WSS --> |JSON messages| WA[Web Application]
        SP --> |Byte stream| HW[Hardware]
        WA --> UI[User Interface]
        HW --> PHY[Physical Board]
    end
    
    classDef process fill:#f96,stroke:#333
    classDef data fill:#9cf,stroke:#333
    classDef endpoint fill:#9f9,stroke:#333
    
    class PGN,FEN data
    class PARSER,BOARD,WSS,SP process
    class WA,HW,UI,PHY endpoint
```

## Architecture Benefits

1. **Decoupled Components**: The emulation layer separates the data source from the presentation layer
2. **Multiple Testing Paths**: Allows testing both web and hardware interfaces
3. **Simulation Mode**: Enables development without physical hardware
4. **Standardized Data**: Uses PGN and FEN standards for data interchange
5. **Extensible Design**: New games can be added by simply adding PGN files

## Technical Implementation

Both emulators share similar code structure:
- They parse command-line arguments for configuration
- They load PGN files and convert them to FEN positions
- They implement error handling and graceful shutdown
- They support looping through positions continuously

The main difference is in how they distribute the FEN strings:
- WebSocket Emulator uses async WebSocket connections
- Serial Emulator uses synchronous serial port communication 