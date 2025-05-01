const builder = require('electron-builder');
const path = require('path');

// Define build configuration
const config = {
  appId: 'com.chess.analyzer',
  productName: 'Chess Analyzer',
  directories: {
    output: path.join(__dirname, 'dist')
  },
  files: [
    '**/*',
    '!**/node_modules/*/{CHANGELOG.md,README.md,README,readme.md,readme}',
    '!**/node_modules/*/{test,__tests__,tests,powered-test,example,examples}',
    '!**/node_modules/*.d.ts',
    '!**/node_modules/.bin',
    '!**/*.{iml,o,hprof,orig,pyc,pyo,rbc,swp,csproj,sln,xproj}',
    '!.editorconfig',
    '!**/._*',
    '!**/{.DS_Store,.git,.hg,.svn,CVS,RCS,SCCS,.gitignore,.gitattributes}',
    '!**/{__pycache__,thumbs.db,.flowconfig,.idea,.vs,.nyc_output}',
    '!**/{appveyor.yml,.travis.yml,circle.yml}',
    '!**/{npm-debug.log,yarn.lock,.yarn-integrity,.yarn-metadata.json}',
    '!**/node_modules/*',
    'node_modules/electron/**',
    'node_modules/express/**',
    'node_modules/chess.js/**',
    'node_modules/path/**',
    'node_modules/dotenv/**',
    'node_modules/node-fetch/**',
    'node_modules/pgn-parser/**',
    'freechess/dist/**',
    'freechess/src/public/**',
    'freechess/src/resources/**',
    'preload.js',
    'main.js',
    '.env'
  ],
  win: {
    target: 'portable',
    icon: path.join(__dirname, 'icon.ico')
  }
};

// Build for Windows
builder.build({
  targets: builder.Platform.WINDOWS.createTarget(),
  config: config
})
.then(() => {
  console.log('Build completed successfully!');
})
.catch((error) => {
  console.error('Error during build:', error);
});
