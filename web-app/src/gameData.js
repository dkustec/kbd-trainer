// Game constants and data structures based on the C implementation

export const GAME_DIRECTIONS = {
  NEUTRAL: 'NEUTRAL',
  UP: 'UP',
  DOWN: 'DOWN',
  FORWARD: 'FORWARD',
  BACK: 'BACK',
  UP_FORWARD: 'UP_FORWARD',
  UP_BACK: 'UP_BACK',
  DOWN_FORWARD: 'DOWN_FORWARD',
  DOWN_BACK: 'DOWN_BACK'
};

export const INPUT_ACCURACY = {
  NONE: 'NONE',
  SUCCESS: 'SUCCESS',
  FAIL: 'FAIL'
};

// Game modes directly translated from C code
export const GAME_MODES = [
  {
    id: 0,
    name: "P1 KBD",
    description: "Korean Back Dash - Player 1 Side",
    pattern: [
      GAME_DIRECTIONS.BACK,
      GAME_DIRECTIONS.NEUTRAL,
      GAME_DIRECTIONS.BACK,
      GAME_DIRECTIONS.DOWN_BACK
    ],
    difficulty: "BEGINNER",
    color: "#ffd700" // Gold
  },
  {
    id: 1,
    name: "P2 KBD",
    description: "Korean Back Dash - Player 2 Side",
    pattern: [
      GAME_DIRECTIONS.FORWARD,
      GAME_DIRECTIONS.NEUTRAL,
      GAME_DIRECTIONS.FORWARD,
      GAME_DIRECTIONS.DOWN_FORWARD
    ],
    difficulty: "BEGINNER",
    color: "#00ffff" // Cyan
  },
  {
    id: 2,
    name: "P1 WD",
    description: "Wave Dash - Player 1 Side",
    pattern: [
      GAME_DIRECTIONS.FORWARD,
      GAME_DIRECTIONS.NEUTRAL,
      GAME_DIRECTIONS.DOWN,
      GAME_DIRECTIONS.DOWN_FORWARD,
      GAME_DIRECTIONS.FORWARD,
      GAME_DIRECTIONS.NEUTRAL
    ],
    difficulty: "INTERMEDIATE",
    color: "#ff6b35" // Orange
  },
  {
    id: 3,
    name: "P2 WD",
    description: "Wave Dash - Player 2 Side",
    pattern: [
      GAME_DIRECTIONS.BACK,
      GAME_DIRECTIONS.NEUTRAL,
      GAME_DIRECTIONS.DOWN,
      GAME_DIRECTIONS.DOWN_BACK,
      GAME_DIRECTIONS.BACK,
      GAME_DIRECTIONS.NEUTRAL
    ],
    difficulty: "INTERMEDIATE",
    color: "#8a2be2" // Purple
  }
];

// Direction symbols for display
export const DIRECTION_SYMBOLS = {
  [GAME_DIRECTIONS.NEUTRAL]: '○',
  [GAME_DIRECTIONS.UP]: '▲',
  [GAME_DIRECTIONS.DOWN]: '▼',
  [GAME_DIRECTIONS.FORWARD]: '►',
  [GAME_DIRECTIONS.BACK]: '◄',
  [GAME_DIRECTIONS.UP_FORWARD]: '◥',
  [GAME_DIRECTIONS.UP_BACK]: '◤',
  [GAME_DIRECTIONS.DOWN_FORWARD]: '◢',
  [GAME_DIRECTIONS.DOWN_BACK]: '◣'
};

// Gamepad button mappings
export const GAMEPAD_MAPPING = {
  // Standard gamepad layout
  BUTTONS: {
    A: 0,        // Cross/A - Select
    B: 1,        // Circle/B - Back
    X: 2,        // Square/X
    Y: 3,        // Triangle/Y
    LB: 4,       // L1/LB
    RB: 5,       // R1/RB
    LT: 6,       // L2/LT
    RT: 7,       // R2/RT
    SELECT: 8,   // Select/View
    START: 9,    // Start/Menu
    LS: 10,      // Left stick click
    RS: 11       // Right stick click
  },
  AXES: {
    LEFT_X: 0,   // Left stick X-axis
    LEFT_Y: 1,   // Left stick Y-axis
    RIGHT_X: 2,  // Right stick X-axis
    RIGHT_Y: 3   // Right stick Y-axis
  }
};

// Score system
export const SCORING = {
  CORRECT_INPUT: 50,
  PERFECT_PATTERN: 100,
  COMBO_MULTIPLIER: 1.5,
  TIME_BONUS: 10
};

// Timing constants (in milliseconds)
export const TIMING = {
  FAILURE_TIMEOUT: 2000,    // 2 seconds timeout after failure
  INPUT_DEBOUNCE: 100,      // Minimum time between inputs
  ANIMATION_DURATION: 300   // UI animation duration
};

// Initial game state
export const INITIAL_GAME_STATE = {
  selectedMode: 0,
  playerPos: 0,
  score: 0,
  highScore: 0,
  currentInput: GAME_DIRECTIONS.NEUTRAL,
  lastInputAccuracy: INPUT_ACCURACY.NONE,
  lastInput: GAME_DIRECTIONS.NEUTRAL,
  failureTimeoutStart: 0,
  inFailureTimeout: false,
  runGame: false,
  combo: 0,
  perfectInputs: 0,
  totalInputs: 0,
  startTime: 0,
  gamepadConnected: false,
  gamepadIndex: -1
};

// Local storage keys
export const STORAGE_KEYS = {
  HIGH_SCORES: 'kbd-trainer-highscores',
  SETTINGS: 'kbd-trainer-settings',
  STATS: 'kbd-trainer-stats'
};

export default {
  GAME_DIRECTIONS,
  INPUT_ACCURACY,
  GAME_MODES,
  DIRECTION_SYMBOLS,
  GAMEPAD_MAPPING,
  SCORING,
  TIMING,
  INITIAL_GAME_STATE,
  STORAGE_KEYS
};
