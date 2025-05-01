# Chess Game Analysis Tool

This is an Electron-based wrapper for the Freechess analysis tool, integrated with the Chess Game application.

## Setup Instructions

1. Make sure you have Node.js installed (version 14 or higher)
2. Run the `setup.bat` file to install all dependencies and set up the project
3. (Optional) Run the `build.bat` file to create a standalone executable

## How to Use

The analysis tool will be automatically launched when you click "Proceed to Analysis" in the game over screen after a chess match. The PGN (Portable Game Notation) of your game will be automatically loaded into the analysis tool.

### Manual Launch

You can also manually launch the analysis tool:

1. From the command line: `cd chessAnalysis && npm start`
2. If you've built the executable: run `chessAnalysis/dist/win-unpacked/Chess Analyzer.exe`

### Analysis Features

- Analyze chess games move by move
- See engine evaluations for each position
- View suggested best moves
- Get move classifications (brilliant, good, mistake, blunder, etc.)
- See accuracy percentages for both players

## Troubleshooting

If you encounter any issues:

1. Make sure you've run `setup.bat` to install all dependencies
2. Check that Node.js is properly installed and in your PATH
3. If the analysis tool doesn't launch automatically, try running it manually
4. If you see errors about missing modules, run `cd chessAnalysis && npm install` to reinstall dependencies

## Credits

This tool uses the Freechess open-source project for chess analysis.
