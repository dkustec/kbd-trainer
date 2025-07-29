import React, { useState, useEffect, useCallback } from 'react';
import {
  GAME_MODES,
  INITIAL_GAME_STATE,
  INPUT_ACCURACY,
  GAME_DIRECTIONS,
  SCORING,
  TIMING,
  STORAGE_KEYS
} from './gameData.js';
import useGamepad from './hooks/useGamepad.js';
import PatternDisplay from './components/PatternDisplay.jsx';
import ScoreBoard from './components/ScoreBoard.jsx';
import ModeSelector from './components/ModeSelector.jsx';
import StatusBar from './components/StatusBar.jsx';

function App() {
  const [gameState, setGameState] = useState(INITIAL_GAME_STATE);
  const [highScores, setHighScores] = useState({});
  const [prevInput, setPrevInput] = useState({
    direction: GAME_DIRECTIONS.NEUTRAL,
    selectPressed: false,
    backPressed: false
  });

  const gamepadState = useGamepad();

  // Load high scores from localStorage
  useEffect(() => {
    const savedScores = localStorage.getItem(STORAGE_KEYS.HIGH_SCORES);
    if (savedScores) {
      setHighScores(JSON.parse(savedScores));
    }
  }, []);

  // Save high scores to localStorage
  const saveHighScores = useCallback((scores) => {
    localStorage.setItem(STORAGE_KEYS.HIGH_SCORES, JSON.stringify(scores));
    setHighScores(scores);
  }, []);

  // Start a new game
  const startGame = useCallback(() => {
    const selectedMode = GAME_MODES[gameState.selectedMode];
    const modeHighScore = highScores[selectedMode.id] || 0;

    setGameState(prev => ({
      ...prev,
      playerPos: 0,
      score: 0,
      highScore: modeHighScore,
      lastInput: GAME_DIRECTIONS.NEUTRAL,
      lastInputAccuracy: INPUT_ACCURACY.NONE,
      inFailureTimeout: false,
      failureTimeoutStart: 0,
      runGame: true,
      combo: 0,
      perfectInputs: 0,
      totalInputs: 0,
      startTime: performance.now()
    }));
  }, [gameState.selectedMode, highScores]);

  // Update menu navigation
  const updateMenu = useCallback((input) => {
    // Check if input has changed
    if (
      input.direction === prevInput.direction &&
      input.selectPressed === prevInput.selectPressed &&
      input.backPressed === prevInput.backPressed
    ) {
      return;
    }

    setPrevInput(input);

    // Start game
    if (input.selectPressed) {
      startGame();
      return;
    }

    // Navigate modes
    if (input.direction === GAME_DIRECTIONS.FORWARD) {
      setGameState(prev => ({
        ...prev,
        selectedMode: prev.selectedMode === GAME_MODES.length - 1 ? 0 : prev.selectedMode + 1
      }));
    } else if (input.direction === GAME_DIRECTIONS.BACK) {
      setGameState(prev => ({
        ...prev,
        selectedMode: prev.selectedMode === 0 ? GAME_MODES.length - 1 : prev.selectedMode - 1
      }));
    }
  }, [prevInput, startGame]);

  // Handle failure timeout
  const handleFailureTimeout = useCallback(() => {
    setGameState(prev => ({
      ...prev,
      inFailureTimeout: true,
      failureTimeoutStart: performance.now()
    }));

    setTimeout(() => {
      setGameState(prev => ({
        ...prev,
        inFailureTimeout: false,
        playerPos: 0,
        score: 0,
        combo: 0,
        lastInputAccuracy: INPUT_ACCURACY.NONE
      }));
    }, TIMING.FAILURE_TIMEOUT);
  }, []);

  // Update game logic
  const updateGame = useCallback((input) => {
    const currentMode = GAME_MODES[gameState.selectedMode];

    // Check if we're in failure timeout
    if (gameState.inFailureTimeout) {
      return;
    }

    // Exit to menu
    if (input.backPressed) {
      const newHighScores = { ...highScores };
      if (gameState.score > (newHighScores[currentMode.id] || 0)) {
        newHighScores[currentMode.id] = gameState.score;
        saveHighScores(newHighScores);
      }

      setGameState(prev => ({
        ...prev,
        runGame: false
      }));
      return;
    }

    // No update if input hasn't changed
    if (input.direction === prevInput.direction) {
      return;
    }

    const expectedDirection = currentMode.pattern[gameState.playerPos];
    const currentTime = performance.now();

    setGameState(prev => {
      const newState = { ...prev };
      newState.totalInputs++;

      if (input.direction === expectedDirection) {
        // Correct input
        let points = SCORING.CORRECT_INPUT;

        // Combo bonus
        if (newState.combo > 0) {
          points = Math.floor(points * (1 + newState.combo * 0.1));
        }

        newState.score += points;
        newState.combo++;
        newState.perfectInputs++;

        // Update high score
        if (newState.score > newState.highScore) {
          newState.highScore = newState.score;
        }

        // Move to next position in pattern
        if (newState.playerPos === currentMode.pattern.length - 1) {
          newState.playerPos = 0;
          // Pattern completion bonus
          newState.score += SCORING.PERFECT_PATTERN;
        } else {
          newState.playerPos++;
        }

        newState.lastInput = input.direction;
        newState.lastInputAccuracy = INPUT_ACCURACY.SUCCESS;
      } else {
        // Incorrect input
        newState.lastInput = input.direction;
        newState.lastInputAccuracy = INPUT_ACCURACY.FAIL;
        newState.combo = 0;

        // Only trigger failure if not at start position
        if (newState.playerPos !== 0) {
          // Trigger failure timeout
          setTimeout(() => handleFailureTimeout(), 100);
        }
      }

      return newState;
    });

    setPrevInput(input);
  }, [gameState, prevInput, highScores, saveHighScores, handleFailureTimeout]);

  // Main update loop
  useEffect(() => {
    if (gamepadState.connected) {
      const input = {
        direction: gamepadState.direction,
        selectPressed: gamepadState.selectPressed,
        backPressed: gamepadState.backPressed
      };

      if (gameState.runGame) {
        updateGame(input);
      } else {
        updateMenu(input);
      }
    }
  }, [gamepadState, gameState.runGame, updateGame, updateMenu]);

  // Keyboard fallback support
  useEffect(() => {
    const handleKeyPress = (e) => {
      if (gamepadState.connected) return; // Prefer gamepad when available

      let direction = GAME_DIRECTIONS.NEUTRAL;
      let selectPressed = false;
      let backPressed = false;

      switch (e.key.toLowerCase()) {
        case 'arrowup':
        case 'w':
          direction = GAME_DIRECTIONS.UP;
          break;
        case 'arrowdown':
        case 's':
          direction = GAME_DIRECTIONS.DOWN;
          break;
        case 'arrowleft':
        case 'a':
          direction = GAME_DIRECTIONS.BACK;
          break;
        case 'arrowright':
        case 'd':
          direction = GAME_DIRECTIONS.FORWARD;
          break;
        case 'enter':
        case ' ':
          selectPressed = true;
          break;
        case 'escape':
        case 'backspace':
          backPressed = true;
          break;
      }

      const input = { direction, selectPressed, backPressed };

      if (gameState.runGame) {
        updateGame(input);
      } else {
        updateMenu(input);
      }
    };

    window.addEventListener('keydown', handleKeyPress);
    return () => window.removeEventListener('keydown', handleKeyPress);
  }, [gamepadState.connected, gameState.runGame, updateGame, updateMenu]);

  const currentMode = GAME_MODES[gameState.selectedMode];

  return (
    <div className="tournament-arena">
      {/* Header */}
      <header className="tournament-header">
        <div>
          <h1 className="tournament-title">
            ⚡ KBD TRAINER
          </h1>
          <p className="tournament-subtitle">Fighting Game Tournament</p>
        </div>
        <div className="flex items-center gap-4">
          🏆
          <span className="text-sm text-muted">Tournament Mode</span>
        </div>
      </header>

      {/* Main Pattern Display */}
      <main className="pattern-arena">
        <PatternDisplay
          pattern={currentMode.pattern}
          currentPosition={gameState.playerPos}
          lastInputAccuracy={gameState.lastInputAccuracy}
          inGame={gameState.runGame}
        />

        {!gameState.runGame && (
          <div className="text-center mt-8">
            <p className="text-lg text-muted mb-2">Selected Mode:</p>
            <h2 className="text-2xl font-bold text-gold mb-1">{currentMode.name}</h2>
            <p className="text-sm text-muted mb-4">{currentMode.description}</p>
            <p className="text-xs text-cyan">
              {gamepadState.connected ? 'Press A to Start' : 'Press Enter to Start'}
            </p>
          </div>
        )}

        {gameState.inFailureTimeout && (
          <div className="absolute inset-0 bg-black bg-opacity-75 flex items-center justify-center">
            <div className="text-center">
              <div className="text-4xl text-red mb-4">FAILURE</div>
              <div className="text-lg text-muted">Resetting in 2 seconds...</div>
            </div>
          </div>
        )}
      </main>

      {/* Stats Panel */}
      <aside className="stats-panel">
        <ScoreBoard
          score={gameState.score}
          highScore={gameState.highScore}
          combo={gameState.combo}
          accuracy={gameState.totalInputs > 0 ? (gameState.perfectInputs / gameState.totalInputs) * 100 : 0}
        />
      </aside>

      {/* Mode Selection */}
      <section className="mode-selection">
        <ModeSelector
          modes={GAME_MODES}
          selectedMode={gameState.selectedMode}
          highScores={highScores}
          disabled={gameState.runGame}
          onModeSelect={(modeId) => {
            if (!gameState.runGame) {
              setGameState(prev => ({ ...prev, selectedMode: modeId }));
            }
          }}
        />
      </section>

      {/* Status Bar */}
      <footer className="status-bar">
        <StatusBar
          gamepadConnected={gamepadState.connected}
          gamepadId={gamepadState.id}
          gameTime={gameState.runGame ? performance.now() - gameState.startTime : 0}
          isGameRunning={gameState.runGame}
        />
      </footer>
    </div>
  );
}

export default App;
