/**
 * KBD Trainer Web - Main Application
 * Simple coordination of game components
 */

class KBDTrainerApp {
    constructor() {
        this.gameEngine = null;
        this.inputHandler = null;
        this.uiController = null;
        
        this.init();
    }
    
    init() {
        console.log('Initializing KBD Trainer Web...');
        
        // Initialize game engine
        this.gameEngine = new GameEngine();
        
        // Initialize UI controller
        this.uiController = new UIController(this.gameEngine);
        
        // Initialize input handler
        this.inputHandler = new InputHandler(this.gameEngine);
        
        // Expose input handler globally for game engine access
        window.inputHandler = this.inputHandler;
        
        // Set default mode selection (P1 KBD - mode 0)
        console.log('App init - about to call selectMode(0), current selectedMode:', this.gameEngine.selectedMode);
        this.uiController.selectMode(0);
        console.log('App init - after selectMode(0), selectedMode:', this.gameEngine.selectedMode);
        
        console.log('KBD Trainer Web initialized successfully!');
        console.log('Controls:');
        console.log('  Keyboard: Arrow Keys, WASD, Numpad');
        console.log('  Gamepad: D-Pad, Left Stick');
        console.log('  Space: Start/Stop');
        console.log('  R: Reset');
        console.log('  1-4: Mode Select');
        
        // Expose to global scope for debugging
        window.kbdTrainer = () => this;
    }
}

// Initialize app when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    const app = new KBDTrainerApp();
    
    // Global debug functions
    window.debugKBD = {
        getEngine: () => app.gameEngine,
        getStats: () => ({
            mode: app.gameEngine.selectedMode,
            modeName: app.gameEngine.getCurrentMode().name,
            playing: app.gameEngine.runGame,
            score: app.gameEngine.score,
            highscore: app.gameEngine.highscore,
            position: app.gameEngine.playerPos,
            pattern: app.gameEngine.getCurrentMode().pattern,
            status: app.gameEngine.getStatus()
        }),
        simulateInput: (direction) => {
            app.gameEngine.update(direction);
            app.uiController.updateUI();
        },
        setMode: (mode) => {
            app.gameEngine.setMode(mode);
            app.uiController.updateUI();
        },
        start: () => {
            app.gameEngine.startGame();
            app.uiController.updateUI();
        },
        stop: () => {
            app.gameEngine.stopGame();
            app.uiController.updateUI();
        },
        reset: () => {
            app.gameEngine.stopGame();
            app.gameEngine.playerPos = 0;
            app.gameEngine.score = 0;
            app.gameEngine.lastInputAcc = 'none';
            app.uiController.updateUI();
        },
        // Gamepad debugging
        getGamepads: () => {
            const gamepads = navigator.getGamepads();
            return Array.from(gamepads).filter(gp => gp).map(gp => ({
                id: gp.id,
                index: gp.index,
                mapping: gp.mapping,
                axes: Array.from(gp.axes),
                buttons: Array.from(gp.buttons).map((btn, i) => ({ index: i, pressed: btn.pressed, value: btn.value }))
            }));
        },
        monitorGamepad: () => {
            const monitor = () => {
                const gamepads = navigator.getGamepads();
                if (gamepads[0]) {
                    const gp = gamepads[0];
                    console.log(`Axes: [${gp.axes.map(a => a.toFixed(2)).join(', ')}]`);
                    const pressedButtons = gp.buttons.map((btn, i) => btn.pressed ? i : null).filter(i => i !== null);
                    if (pressedButtons.length > 0) {
                        console.log(`Pressed buttons: [${pressedButtons.join(', ')}]`);
                    }
                }
                requestAnimationFrame(monitor);
            };
            monitor();
        }
    };
    
    console.log('Debug functions available:');
    console.log('  debugKBD.getStats() - Get current game state');
    console.log('  debugKBD.simulateInput(direction) - Simulate input');
    console.log('  debugKBD.setMode(0-3) - Set mode');
    console.log('  debugKBD.start() - Start game');
    console.log('  debugKBD.stop() - Stop game');
    console.log('  debugKBD.reset() - Reset game');
});
