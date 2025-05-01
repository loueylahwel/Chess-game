const { contextBridge, ipcRenderer } = require('electron');

// Expose IPC API to renderer process
contextBridge.exposeInMainWorld('electronAPI', {
  sendPGN: (pgn) => ipcRenderer.send('send-pgn', pgn)
});

// Listen for PGN data from main process
ipcRenderer.on('load-pgn', (event, pgn) => {
  console.log('Received PGN data:', pgn);

  // Function to set PGN and click analyze button
  const setPgnAndAnalyze = () => {
    console.log('Setting PGN in textarea');
    // Try different possible IDs for the PGN textarea
    const pgnTextarea = document.getElementById('pgn') ||
                        document.querySelector('textarea[name="pgn"]') ||
                        document.querySelector('textarea');

    if (pgnTextarea) {
      console.log('Found PGN textarea, setting value');
      pgnTextarea.value = pgn;

      // Automatically click the analyze button after a short delay
      setTimeout(() => {
        console.log('Looking for analyze button');
        // Try different possible IDs/classes for the analyze button
        const analyzeButton = document.getElementById('review-button') ||
                             document.querySelector('button[type="submit"]') ||
                             document.querySelector('input[type="submit"]') ||
                             document.querySelector('button.analyze') ||
                             document.querySelector('button');

        if (analyzeButton) {
          console.log('Found analyze button, clicking');
          analyzeButton.click();
        } else {
          console.log('Could not find analyze button');
        }
      }, 2000);
    } else {
      console.log('Could not find PGN textarea');
    }
  };

  // If DOM is already loaded, set PGN immediately
  if (document.readyState === 'complete' || document.readyState === 'interactive') {
    console.log('DOM already loaded, setting PGN immediately');
    setPgnAndAnalyze();
  } else {
    // Wait for DOM to be ready
    console.log('Waiting for DOM to be ready');
    document.addEventListener('DOMContentLoaded', () => {
      console.log('DOM now loaded, setting PGN');
      // Give a bit more time for all scripts to initialize
      setTimeout(setPgnAndAnalyze, 1000);
    });
  }
});
