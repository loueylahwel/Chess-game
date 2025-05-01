# Chess Game with Analysis Tool

A comprehensive chess application that combines a playable chess game with powerful analysis capabilities. Built with SFML for the game interface and Electron for the analysis tool.

## Features

- **Chess Game**
  - Full chess game implementation with all standard rules
  - Support for special moves (castling, en passant, pawn promotion)
  - Multiple game modes:
    - Player vs Player
    - Player vs Computer (Stockfish integration)
    - LAN multiplayer (Host/Client)
  - Beautiful UI with piece animations and move sounds
  - Move validation and legal move highlighting

- **Analysis Tool**
  - Built-in Stockfish engine analysis
  - Move accuracy evaluation
  - Position evaluation graph
  - Move classification (Best, Excellent, Good, Inaccuracy, Mistake, Blunder)
  - Opening recognition
  - Cloud evaluation integration with Lichess
  - PGN import/export support

## Prerequisites

- Windows OS
- C++ compiler (MinGW-w64 recommended)
- Node.js and npm
- SFML library
- Stockfish chess engine

## Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/Chess-game.git
cd Chess-game
```

2. Install SFML dependencies:
   - Make sure SFML libraries are in the root directory:
     - sfml-audio-2.dll
     - sfml-graphics-2.dll
     - sfml-network-2.dll
     - sfml-system-2.dll
     - sfml-window-2.dll

3. Install Node.js dependencies for the analysis tool:
```bash
cd chessAnalysis
npm install
```

4. Make sure Stockfish is available in the root directory as `stockfish.exe`

## Usage

### Playing Chess

1. Run the chess game by executing `main.exe` or compiling and running with:
```bash
make
./main.exe
```

2. Select your game mode:
   - Two Player: Play locally against another person
   - vs Computer: Play against Stockfish engine with adjustable difficulty
   - LAN Game: Play over local network

### Analyzing Games

1. After a game is finished, click the "Analyze" button to launch the analysis tool
2. The game will be automatically loaded into the analyzer
3. You can also manually load PGN files for analysis

### Controls

- Left-click to select and move pieces
- Valid moves will be highlighted
- Use the analysis tool's playback controls to review moves:
  - ⏮️ Back to start
  - ◀️ Previous move
  - ▶️ Next move
  - ⏭️ Go to end
  - 🔄 Flip board

## Project Structure

- `/src` - Main chess game source code
- `/coding` - Game assets and resources
  - `/images` - Piece and UI images
  - `/sounds` - Game sound effects
  - `/font` - UI fonts
- `/chessAnalysis` - Analysis tool source code
  - `/freechess` - Analysis engine and UI

## Building from Source

1. Compile the chess game:
```bash
make
```

2. Build the analysis tool:
```bash
cd chessAnalysis
npm run build
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- SFML library
- Stockfish.org chess engine
- Electron framework
WintrCat for the amazing post-game analysis 
https://github.com/WintrCat
