/**
 * KBD Trainer Web - Core Game Engine
 * Exact replica of original C logic
 */

class GameEngine {
    // Debug flag
    static DEBUG = true;
    // Constants
    static SCORE_PER_INPUT = 50;
    static MISS_PAUSE_SECONDS = 2;
    static MISS_PAUSE_INTERVAL = 100;
    static MISS_PAUSE_DECREMENT = 0.1;

    constructor() {
        // Game modes - exact replica of original C code
        this.modes = {
            0: {
                name: 'P1 KBD',
                pattern: ['back', 'neutral', 'back', 'down-back'],
                patternSize: 4
            },
            1: {
                name: 'P2 KBD', 
                pattern: ['forward', 'neutral', 'forward', 'down-forward'],
                patternSize: 4
            },
            2: {
                name: 'P1 WD',
                pattern: ['forward', 'neutral', 'down', 'down-forward', 'forward', 'neutral'],
                patternSize: 6
            },
            3: {
                name: 'P2 WD',
                pattern: ['back', 'neutral', 'down', 'down-back', 'back', 'neutral'],
                patternSize: 6
            }
        };
        
        // Game state - exact replica of original GameState struct
        this.selectedMode = 0;
        this.runGame = false;
        this.playerPos = 0;
        this.score = 0;
        this.highscore = 0;
        this.currInput = 'neutral';
        this.lastInputAcc = 'none'; // none, success, fail
        this.lastInput = 'neutral';
        this.missTime = 0;
        this.inMissPause = false;
        this.missCountdown = 0; // Countdown timer for miss pause
        this._missPauseTimer = null;
        
        // Enhanced stats tracking
        this.totalInputs = 0;
        this.perfectInputs = 0;
        this.missedInputs = 0;
        this.currentCombo = 0;
        this.maxCombo = 0;
        this.accuracy = 100;
        
        // Input tracking
        this.prevDirection = 'neutral';
        this.ignoreNextInput = false; // Flag to ignore stale input after miss pause reset
        
        // Highscores for each mode
        this.highscores = [0, 0, 0, 0];
    }
    
    getCurrentMode() {
        return this.modes[this.selectedMode];
    }
    
    setMode(modeIndex) {
        if (modeIndex >= 0 && modeIndex < Object.keys(this.modes).length) {
            this.selectedMode = modeIndex;
            this.highscore = this.highscores[this.selectedMode];
        }
    }
    
    startGame() {
        if (GameEngine.DEBUG) console.log(`Mode(${this.selectedMode}) score: ${this.highscores[this.selectedMode]}`);
        this.playerPos = 0;
        this.score = 0;
        this.highscore = this.highscores[this.selectedMode];
        this.lastInput = 'neutral';
        this.lastInputAcc = 'none';
        this.missTime = 0;
        this.inMissPause = false;
        this.missCountdown = 0;
        this.runGame = true;
        this.prevDirection = 'neutral';
        
        // Reset input handler state for clean start
        if (this.inputHandler) {
            this.inputHandler.resetInputState();
        }
    }
    
    stopGame() {
        this.highscores[this.selectedMode] = this.highscore;
        this.runGame = false;
        this.clearMissPauseTimer();
    }
    
    update(direction) {
        if (this.runGame) {
            if (direction) {
                this.updateGame(direction);
            }
        } else {
            this.updateMenu(direction);
        }
    }
    
    // Simplified game state update (no longer needs continuous miss pause countdown)
    updateGameState() {
        // This method is now primarily for UI controller calls
        // Miss pause is handled by setTimeout in updateGame()
    }
    
    updateMenu(direction) {
        // Only process if direction changed
        if (direction === this.prevDirection) return;
        this.prevDirection = direction;
        
        if (direction === 'forward') {
            if (this.selectedMode === 3) {
                this.selectedMode = 0;
            } else {
                this.selectedMode += 1;
            }
        } else if (direction === 'back') {
            if (this.selectedMode === 0) {
                this.selectedMode = 3;
            } else {
                this.selectedMode -= 1;
            }
        }
    }
    
    updateGame(direction) {
        this.currInput = direction;
        
        // Don't process new input during miss pause
        if (this.inMissPause) {
            return;
        }
        
        // Only process if direction changed (prevent duplicate processing)
        if (direction === this.prevDirection) {
            return;
        }
        this.prevDirection = direction;
        
        // Check if we just had a failed input and need to start miss pause
        if (this.lastInputAcc === 'fail') {
            // Start miss pause timer
            this.inMissPause = true;
            this.missCountdown = GameEngine.MISS_PAUSE_SECONDS;
            this.lastInputAcc = 'none'; // Clear immediately to prevent UI failure state during pause
            this.clearMissPauseTimer();
            this._missPauseTimer = setInterval(() => {
                if (this.inMissPause) {
                    this.missCountdown -= GameEngine.MISS_PAUSE_DECREMENT;
                    if (this.missCountdown <= 0) {
                        this.clearMissPauseTimer();
                        this.playerPos = 0;
                        this.score = 0; // Reset score to 0 as per original C logic
                        this.lastInputAcc = 'none';
                        this.inMissPause = false;
                        this.missCountdown = 0;
                        this.prevDirection = 'neutral';
                        this.lastInput = 'neutral';
                        // Reset input handler state
                        if (this.inputHandler) {
                            this.inputHandler.resetInputState();
                        }
                        // Clear all UI state and force updates
                        if (this.uiController) {
                            this.uiController.updateUI();
                        }
                    }
                } else {
                    this.clearMissPauseTimer();
                }
            }, GameEngine.MISS_PAUSE_INTERVAL);
            return;
        }
        
        // Input processing
        const currentMode = this.getCurrentMode();
        const expectedInput = currentMode.pattern[this.playerPos % currentMode.patternSize];
        if (direction === expectedInput) {
            // Correct input
            this.totalInputs++;
            this.perfectInputs++;
            this.currentCombo++;
            if (this.currentCombo > this.maxCombo) {
                this.maxCombo = this.currentCombo;
            }
            this.score += GameEngine.SCORE_PER_INPUT;
            if (this.score > this.highscore) {
                this.highscore = this.score;
            }
            if (this.playerPos === currentMode.patternSize - 1) {
                this.playerPos = 0;
                this.lastInput = 'neutral';
                this.lastInputAcc = 'none';
                setTimeout(() => {
                    if (this.uiController) {
                        this.uiController.updateUI();
                    }
                }, 50);
            } else {
                this.playerPos += 1;
                this.lastInput = direction;
                this.lastInputAcc = 'success';
            }
            this.updateAccuracy();
        } else {
            // Wrong input
            this.totalInputs++;
            this.missedInputs++;
            this.currentCombo = 0;
            this.lastInput = direction;
            this.lastInputAcc = 'fail';
            if (GameEngine.DEBUG) console.log(`Wrong input at position ${this.playerPos}: got ${direction}, expected ${expectedInput} - triggering miss pause`);
            this.updateAccuracy();
        }
    }
    
    // Enhanced accuracy calculation
    updateAccuracy() {
        if (this.totalInputs === 0) {
            this.accuracy = 100;
        } else {
            this.accuracy = Math.round((this.perfectInputs / this.totalInputs) * 100);
        }
    }
    
    // Enhanced reset with stats preservation option
    resetGame(preserveStats = false) {
        this.runGame = false;
        this.playerPos = 0;
        this.score = 0;
        this.currInput = 'neutral';
        this.lastInputAcc = 'none';
        this.lastInput = 'neutral';
        this.missTime = 0;
        this.inMissPause = false;
        this.missCountdown = 0;
        this.prevDirection = 'neutral';
        
        // Reset input handler state
        if (this.inputHandler) {
            this.inputHandler.resetInputState();
        }
        
        if (!preserveStats) {
            this.totalInputs = 0;
            this.perfectInputs = 0;
            this.missedInputs = 0;
            this.currentCombo = 0;
            this.maxCombo = 0;
            this.accuracy = 100;
        }
    }
    
    // Get enhanced stats for UI
    getStats() {
        return {
            score: this.score,
            highscore: this.highscore,
            accuracy: this.accuracy,
            totalInputs: this.totalInputs,
            perfectInputs: this.perfectInputs,
            missedInputs: this.missedInputs,
            currentCombo: this.currentCombo,
            maxCombo: this.maxCombo
        };
    }
    
    // Helper methods for UI
    getExpectedInput() {
        if (!this.runGame) return null;
        const currentMode = this.getCurrentMode();
        return currentMode.pattern[this.playerPos % currentMode.patternSize];
    }
    
    getProgress() {
        if (!this.runGame) return 0;
        const currentMode = this.getCurrentMode();
        return (this.playerPos / currentMode.patternSize) * 100;
    }
    
    getStatus() {
        if (this.inMissPause) return 'miss_pause';
        if (!this.runGame) return 'menu';
        return 'playing';
    }
    
    // Clear miss pause timer
    clearMissPauseTimer() {
        if (this._missPauseTimer) {
            clearInterval(this._missPauseTimer);
            this._missPauseTimer = null;
        }
    }
}
