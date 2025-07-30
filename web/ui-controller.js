/**
 * KBD Trainer Web - UI Controller
 * Simple, focused UI matching original design priorities
 */

class UIController {
    constructor(gameEngine) {
        this.gameEngine = gameEngine;
        this.init();
    }
    
    init() {
        this.setupEventListeners();
        this.updateUI();
        
        // Update UI regularly - more frequent during miss pause
        setInterval(() => {
            this.updateUI();
            // During miss pause, update more frequently to catch when it ends
            if (this.gameEngine.inMissPause) {
                // Check every 50ms during miss pause for immediate UI response
                setTimeout(() => this.updateUI(), 50);
                setTimeout(() => this.updateUI(), 100);
            }
        }, 100);
        
        // Update gamepad status every 500ms
        setInterval(() => {
            this.updateGamepadPanel();
        }, 500);
        
        // Expose globally
        window.uiController = this;
    }
    
    setupEventListeners() {
        // Mode selection buttons
        document.getElementById('mode-p1-kbd').addEventListener('click', () => this.selectMode(0));
        document.getElementById('mode-p2-kbd').addEventListener('click', () => this.selectMode(1));
        document.getElementById('mode-p1-wd').addEventListener('click', () => this.selectMode(2));
        document.getElementById('mode-p2-wd').addEventListener('click', () => this.selectMode(3));
        
        // Control buttons
        document.getElementById('start-btn').addEventListener('click', () => this.toggleGame());
        document.getElementById('reset-btn').addEventListener('click', () => this.resetGame());
        
        // Gamepad panel toggle
        document.getElementById('gamepad-toggle').addEventListener('click', () => this.toggleGamepadPanel());
    }
    
    selectMode(modeIndex) {
        this.gameEngine.setMode(modeIndex);
        this.updateUI();
    }
    
    toggleGame() {
        if (this.gameEngine.runGame) {
            this.gameEngine.stopGame();
        } else {
            this.gameEngine.startGame();
        }
        this.updateUI();
    }
    
    resetGame() {
        this.gameEngine.stopGame();
        this.gameEngine.playerPos = 0;
        this.gameEngine.score = 0;
        this.gameEngine.lastInputAcc = 'none';
        this.gameEngine.inMissPause = false;
        this.updateUI();
    }
    
    updateUI() {
        // Update mode selection
        this.updateModeButtons();
        
        // Update game display
        this.updateGameDisplay();
        
        // Update controls
        this.updateControls();
    }
    
    updateModeButtons() {
        const buttons = [
            document.getElementById('mode-p1-kbd'),
            document.getElementById('mode-p2-kbd'),
            document.getElementById('mode-p1-wd'),
            document.getElementById('mode-p2-wd')
        ];
        
        buttons.forEach((btn, index) => {
            if (btn) {
                btn.classList.toggle('active', index === this.gameEngine.selectedMode);
            }
        });
    }
    
    updateGameDisplay() {
        // Current mode name
        const modeNameEl = document.getElementById('current-mode');
        if (modeNameEl) {
            modeNameEl.textContent = this.gameEngine.getCurrentMode().name;
        }
        
        // Pattern display
        this.updatePatternDisplay();
        
        // Status display
        this.updateStatusDisplay();
        
        // Score display
        this.updateScoreDisplay();
    }
    
    updatePatternDisplay() {
        const patternEl = document.getElementById('pattern-display');
        if (!patternEl) return;
        
        const currentMode = this.gameEngine.getCurrentMode();
        const pattern = currentMode.pattern;
        
        patternEl.innerHTML = '';
        
        pattern.forEach((direction, index) => {
            const stepEl = document.createElement('div');
            stepEl.className = 'pattern-step';
            stepEl.textContent = this.directionToSymbol(direction);
            
            // Highlight current step
            if (this.gameEngine.runGame && index === this.gameEngine.playerPos) {
                stepEl.classList.add('current');
            }
            
            // Show completed steps
            if (this.gameEngine.runGame && index < this.gameEngine.playerPos) {
                stepEl.classList.add('completed');
            }
            
            patternEl.appendChild(stepEl);
        });
    }
    
    updateStatusDisplay() {
        const statusEl = document.getElementById('game-status');
        if (!statusEl) return;
        
        const status = this.gameEngine.getStatus();
        
        switch (status) {
            case 'menu':
                statusEl.textContent = 'Select mode and press Start';
                statusEl.className = 'game-status status-menu';
                break;
            case 'playing':
                const expected = this.gameEngine.getExpectedInput();
                statusEl.textContent = `Next: ${this.directionToSymbol(expected)}`;
                statusEl.className = 'game-status status-playing';
                break;
            case 'miss_pause':
                const countdown = this.gameEngine.missCountdown;
                if (countdown > 0) {
                    statusEl.innerHTML = `MISS!<br><span class="countdown">${countdown}</span>`;
                } else {
                    statusEl.innerHTML = `MISS!<br><span class="countdown">0</span>`;
                }
                statusEl.className = 'game-status status-miss';
                break;
        }
    }
    
    updateScoreDisplay() {
        const scoreEl = document.getElementById('score');
        const highscoreEl = document.getElementById('highscore');
        
        if (scoreEl) {
            scoreEl.textContent = this.gameEngine.score;
        }
        
        if (highscoreEl) {
            highscoreEl.textContent = this.gameEngine.highscore;
        }
        
        // Update last input
        const lastInputEl = document.getElementById('last-input');
        if (lastInputEl) {
            lastInputEl.textContent = this.directionToSymbol(this.gameEngine.lastInput);
            lastInputEl.className = 'last-input';
            
            if (this.gameEngine.lastInputAcc === 'success') {
                lastInputEl.classList.add('success');
            } else if (this.gameEngine.lastInputAcc === 'fail') {
                lastInputEl.classList.add('fail');
            }
        }
    }
    
    updateControls() {
        const startBtn = document.getElementById('start-btn');
        if (startBtn) {
            startBtn.textContent = this.gameEngine.runGame ? 'Stop' : 'Start';
        }
    }
    
    directionToSymbol(direction) {
        const symbols = {
            'neutral': '●',
            'up': '↑',
            'up-forward': '↗',
            'forward': '→',
            'down-forward': '↘',
            'down': '↓',
            'down-back': '↙',
            'back': '←',
            'up-back': '↖'
        };
        
        return symbols[direction] || '?';
    }
    
    toggleGamepadPanel() {
        const gamepadList = document.getElementById('gamepad-list');
        const toggleBtn = document.getElementById('gamepad-toggle');
        
        if (gamepadList.classList.contains('hidden')) {
            gamepadList.classList.remove('hidden');
            toggleBtn.textContent = 'Hide';
        } else {
            gamepadList.classList.add('hidden');
            toggleBtn.textContent = 'Show';
        }
    }
    
    updateGamepadPanel() {
        const gamepadList = document.getElementById('gamepad-list');
        if (!gamepadList) return;
        
        const gamepads = navigator.getGamepads();
        const connectedGamepads = Array.from(gamepads).filter(gp => gp);
        
        if (connectedGamepads.length === 0) {
            gamepadList.innerHTML = '<div class="gamepad-item">No controllers detected</div>';
            return;
        }
        
        gamepadList.innerHTML = '';
        
        connectedGamepads.forEach((gamepad, index) => {
            const gamepadItem = document.createElement('div');
            gamepadItem.className = 'gamepad-item connected';
            
            gamepadItem.innerHTML = `
                <div class="gamepad-name">${gamepad.id}</div>
                <div class="gamepad-details">
                    Index: ${gamepad.index} | 
                    Mapping: ${gamepad.mapping || 'standard'} | 
                    Buttons: ${gamepad.buttons.length} | 
                    Axes: ${gamepad.axes.length}
                    <span class="gamepad-status connected">Connected</span>
                </div>
            `;
            
            gamepadList.appendChild(gamepadItem);
        });
    }
}
