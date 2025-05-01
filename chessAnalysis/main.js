const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');
const fs = require('fs');
const express = require('express');
const dotenv = require('dotenv');

// Load environment variables
dotenv.config();

// Create Express app
const expressApp = express();
const PORT = process.env.PORT || 3000;

// Set up Express middleware and routes
expressApp.use(express.json());

// Serve static files from freechess/dist and freechess/src
expressApp.use('/static', [
  express.static(path.join(__dirname, 'freechess/dist/public')),
  express.static(path.join(__dirname, 'freechess/src/public'))
]);

// Import and use the Freechess API router
const apiRouter = require('./freechess/dist/api.js');
expressApp.use('/api', apiRouter);

expressApp.get('/', (req, res) => {
  res.sendFile(path.resolve(__dirname, 'freechess/src/public/pages/report/index.html'));
});

// Start Express server
let server;
try {
  server = expressApp.listen(PORT, () => {
    console.log(`Server running on port ${PORT}`);
  });
} catch (error) {
  console.error('Failed to start server:', error);
}

// Electron window
let mainWindow;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1200,
    height: 800,
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: path.join(__dirname, 'preload.js')
    }
  });

  // Load the Express server URL
  mainWindow.loadURL(`http://localhost:${PORT}`);

  // Check for command line arguments (PGN file path)
  const pgnFilePath = process.argv[2];
  if (pgnFilePath && fs.existsSync(pgnFilePath)) {
    try {
      const pgn = fs.readFileSync(pgnFilePath, 'utf8');
      // Wait for window to be ready before sending PGN
      mainWindow.webContents.on('did-finish-load', () => {
        mainWindow.webContents.send('load-pgn', pgn);
      });
    } catch (error) {
      console.error('Error reading PGN file:', error);
    }
  }

  // Open DevTools in development for debugging
  mainWindow.webContents.openDevTools();

  mainWindow.on('closed', () => {
    mainWindow = null;
    if (server) {
      server.close();
    }
  });
}

app.on('ready', () => {
  createWindow();
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  if (mainWindow === null) {
    createWindow();
  }
});

// IPC handlers
ipcMain.on('send-pgn', (event, pgn) => {
  if (mainWindow) {
    mainWindow.webContents.send('load-pgn', pgn);
  }
});
