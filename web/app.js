/**
 * KBD Trainer Web - Main Application
 * Simple coordination of game components
 */

// Toggle DEBUG logging
const DEBUG = false;
class KBDTrainerApp {
    constructor() {
        this.gameEngine = null;
        this.inputHandler = null;
        this.uiController = null;
        
        this.init();
    }
    
    init() {
        if (DEBUG) console.log('Initializing KBD Trainer Web...');
        
        // Initialize game engine
        this.gameEngine = new GameEngine();
        
        // Initialize UI controller
        this.uiController = new UIController(this.gameEngine);
        
        // Initialize input handler
        this.inputHandler = new InputHandler(this.gameEngine);
        
        
        // Set default mode selection (P1 KBD - mode 0)
        if (DEBUG) console.log('App init - about to call selectMode(0), current selectedMode:', this.gameEngine.selectedMode);
        this.uiController.selectMode(0);
        if (DEBUG) console.log('App init - after selectMode(0), selectedMode:', this.gameEngine.selectedMode);
        
        if (DEBUG) console.log('KBD Trainer Web initialized successfully!');
        if (DEBUG) {
            console.log('Controls:');
            console.log('  Keyboard: Arrow Keys, WASD, Numpad');
            console.log('  Gamepad: D-Pad, Left Stick');
            console.log('  Space: Start/Stop');
            console.log('  R: Reset');
            console.log('  1-4: Mode Select');
        }
        
        // Expose single entry point for external interaction
        window.kbdTrainerApp = this;
    }
    
    /**
     * Start the game and update UI.
     */
    start() {
        this.gameEngine.startGame();
        this.uiController.updateUI();
    }

    /**
     * Stop the game and update UI.
     */
    stop() {
        this.gameEngine.stopGame();
        this.uiController.updateUI();
    }

    /**
     * Reset game state and update UI.
     */
    reset() {
        this.gameEngine.stopGame();
        this.gameEngine.playerPos = 0;
        this.gameEngine.score = 0;
        this.gameEngine.lastInputAcc = 'none';
        this.uiController.updateUI();
    }

    /**
     * Change mode and update UI.
     */
    setMode(mode) {
        this.gameEngine.setMode(mode);
        this.uiController.updateUI();
    }

    /**
     * Simulate a user input for debugging.
     */
    simulateInput(direction) {
        this.gameEngine.update(direction);
        this.uiController.updateUI();
    }

    /**
     * Return current game stats.
     */
    getStats() {
        return {
            mode: this.gameEngine.selectedMode,
            modeName: this.gameEngine.getCurrentMode().name,
            playing: this.gameEngine.runGame,
            score: this.gameEngine.score,
            highscore: this.gameEngine.highscore,
            position: this.gameEngine.playerPos,
            pattern: this.gameEngine.getCurrentMode().pattern,
            status: this.gameEngine.getStatus()
        };
    }

    /**
     * Return connected gamepads info.
     */
    getGamepads() {
        return Array.from(navigator.getGamepads() || [])
            .filter(gp => gp)
            .map(gp => ({
                id: gp.id,
                index: gp.index,
                axes: [...gp.axes],
                buttons: gp.buttons.map((btn, i) => ({ index: i, pressed: btn.pressed, value: btn.value }))
            }));
    }

    /**
     * Monitor gamepad inputs at a throttled rate.
     */
    monitorGamepad(interval = 500) {
        let lastTime = 0;
        const monitor = (ts) => {
            if (ts - lastTime > interval) {
                const gp = (navigator.getGamepads() || [])[0];
                if (gp && DEBUG) {
                    console.log(`Axes: [${gp.axes.map(a => a.toFixed(2)).join(', ')}]`);
                    const btns = gp.buttons.map((b,i) => b.pressed ? i : null).filter(i=>i!=null);
                    if (btns.length) console.log(`Buttons: [${btns.join(', ')}]`);
                }
                lastTime = ts;
            }
            this._rafId = requestAnimationFrame(monitor);
        };
        this._rafId = requestAnimationFrame(monitor);
    }
  
  /**
   * Stop gamepad monitoring.
   */
  cancelGamepadMonitor() {
    if (this._rafId) {
      cancelAnimationFrame(this._rafId);
      this._rafId = null;
    }
  }
  
  /**
   * Cleanup event listeners and loops.
   */
  destroy() {
    this.cancelGamepadMonitor();
    // Remove window references
    delete window.kbdTrainerApp;
    delete window.inputHandler;
    // Other cleanup if needed
    if (DEBUG) console.log('KBDTrainerApp destroyed.');
  }
}

// Initialize app when DOM is loaded
// Initialize app when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    try {
        const app = new KBDTrainerApp();
        if (DEBUG) {
            console.log('Debugger API available on window.kbdTrainerApp:',
                Object.getOwnPropertyNames(Object.getPrototypeOf(app))
                    .filter(name => typeof app[name] === 'function' && name !== 'constructor')
            );
        }
    } catch (err) {
        console.error('Failed to initialize KBD Trainer App:', err);
    }
});
