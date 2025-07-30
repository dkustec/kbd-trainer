/**
 * KBD Trainer Web - Core Game Engine
 * Exact replica of original C logic
 */

class GameEngine {
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
        
        // Input tracking
        this.prevDirection = 'neutral';
        
        // Highscores for each mode
        this.highscores = [0, 0, 0, 0];
    }
    
    getCurrentMode() {
        return this.modes[this.selectedMode];
    }
    
    setMode(modeIndex) {
        if (modeIndex >= 0 && modeIndex < 4) {
            this.selectedMode = modeIndex;
            return true;
        }
        return false;
    }
    
    startGame() {
        console.log(`Mode(${this.selectedMode}) score: ${this.highscores[this.selectedMode]}`);
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
    }
    
    stopGame() {
        this.highscores[this.selectedMode] = this.highscore;
        this.runGame = false;
    }
    
    update(direction) {
        if (this.runGame) {
            this.updateGame(direction);
        } else {
            this.updateMenu(direction);
        }
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
        
        // Handle miss pause - wait 3 seconds before resetting
        if (this.inMissPause) {
            const currentTime = performance.now();
            const pauseDuration = 3000; // 3 seconds in milliseconds
            const elapsed = currentTime - this.missTime;
            
            // Calculate countdown (3, 2, 1, 0)
            const remaining = pauseDuration - elapsed;
            this.missCountdown = Math.max(0, Math.ceil(remaining / 1000)); // 1000ms per count (3->2->1->0)
            
            if (elapsed >= pauseDuration) {
                // Reset after pause
                this.playerPos = 0;
                this.score = 0;
                this.lastInputAcc = 'none';
                this.inMissPause = false;
                this.missCountdown = 0;
                this.prevDirection = 'neutral'; // Reset to neutral, not current direction
                console.log('Miss pause ended - game reset');
                
                // Force multiple UI updates immediately after miss pause ends
                if (window.uiController) {
                    window.uiController.updateUI();
                    setTimeout(() => window.uiController.updateUI(), 10);
                    setTimeout(() => window.uiController.updateUI(), 50);
                }
            }
            return;
        }
        
        if (this.lastInputAcc === 'fail') {
            // Start miss pause timer
            this.missTime = performance.now();
            this.inMissPause = true;
            console.log('Miss detected - starting 2 second pause');
            return;
        }
        
        // No update if input has not changed
        if (direction === this.prevDirection) return;
        
        const currentMode = this.getCurrentMode();
        const expectedInput = currentMode.pattern[this.playerPos % currentMode.patternSize];
        
        if (direction === expectedInput) {
            // Correct input
            this.score += 50;
            if (this.score > this.highscore) {
                this.highscore = this.score;
            }
            
            if (this.playerPos === currentMode.patternSize - 1) {
                this.playerPos = 0;
            } else {
                this.playerPos += 1;
            }
            
            this.prevDirection = direction;
            this.lastInput = direction;
            this.lastInputAcc = 'success';
        } else {
            // Incorrect input
            this.prevDirection = direction;
            
            if (this.playerPos !== 0) {
                this.lastInput = direction;
                this.lastInputAcc = 'fail';
            }
        }
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
}
