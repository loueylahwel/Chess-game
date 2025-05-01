# Chess Analysis Tool Setup Guide

This guide will help you set up the Chess Analysis Tool properly.

## Prerequisites

1. **Node.js**: Make sure you have Node.js installed (version 14 or higher)
   - Download from: https://nodejs.org/

2. **Git**: Recommended for cloning repositories
   - Download from: https://git-scm.com/downloads

## Step-by-Step Setup

### 1. Copy the Freechess Files

First, make sure the Freechess files are copied to the chessAnalysis directory:

```
cd chessAnalysis
.\copy-freechess.bat
```

### 2. Install Dependencies Manually

If the setup.bat script doesn't work, you can install dependencies manually:

```
cd chessAnalysis
npm install --save-dev electron
npm install

cd freechess
npm install
npm run build
cd ..
```

### 3. Test the Application

Try running the application directly:

```
cd chessAnalysis
npx electron .
```

### 4. Build the Executable (Optional)

If you want to create a standalone executable:

```
cd chessAnalysis
npm install --save-dev electron-builder
npx electron-builder --win
```

## Troubleshooting

### If you encounter EBUSY errors:

1. Close any running instances of the application
2. Restart your computer
3. Try installing dependencies again

### If the application doesn't launch:

1. Check the console for error messages
2. Make sure all dependencies are installed
3. Try running the application with `npx electron .` to see any error output

### If the PGN file doesn't load:

1. Check that the temp_game.pgn file exists in the root directory
2. Make sure the file has proper PGN content

## Manual Analysis

If the analysis tool doesn't work, you can:

1. Use the PGN file that opens in Notepad
2. Copy the PGN content to an online analysis tool like:
   - https://lichess.org/analysis
   - https://chess.com/analysis
