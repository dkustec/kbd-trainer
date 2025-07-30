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
        // Enhanced mode selection cards
        document.querySelectorAll('.mode-card').forEach((card, index) => {
            card.addEventListener('click', () => this.selectMode(index));
        });
        
        // Control buttons
        document.getElementById('start-btn').addEventListener('click', () => this.toggleGame());
        document.getElementById('reset-btn').addEventListener('click', () => this.resetGame());
        
        // Gamepad panel toggle
        document.getElementById('gamepad-toggle').addEventListener('click', () => this.toggleGamepadPanel());
    }
    
    selectMode(modeIndex) {
        this.gameEngine.setMode(modeIndex);
        
        // Update mode card selection
        document.querySelectorAll('.mode-card').forEach((card, index) => {
            if (index === modeIndex) {
                card.classList.add('selected');
            } else {
                card.classList.remove('selected');
            }
        });
        
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
        this.gameEngine.resetGame(); // Use enhanced reset method
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
        
        // Enhanced stats display
        this.updateStatsDisplay();
        
        // Combo display
        this.updateComboDisplay();
    }
    
    updatePatternDisplay() {
        const patternEl = document.getElementById('pattern-display');
        if (!patternEl) return;
        
        const currentMode = this.gameEngine.getCurrentMode();
        const pattern = currentMode.pattern;
        
        patternEl.innerHTML = '';
        
        pattern.forEach((direction, index) => {
            const stepEl = document.createElement('div');
            stepEl.className = 'direction-icon';
            stepEl.innerHTML = `<span class="icon-symbol">${this.directionToSymbol(direction)}</span>`;
            
            // Determine state based on game progress
            if (this.gameEngine.runGame) {
                if (index === this.gameEngine.playerPos) {
                    stepEl.classList.add('active');
                } else if (index < this.gameEngine.playerPos) {
                    stepEl.classList.add('completed');
                } else {
                    stepEl.classList.add('neutral');
                }
                
                // Add failure animation if last input was wrong at this position
                if (this.gameEngine.lastInputAcc === 'fail' && index === this.gameEngine.playerPos) {
                    stepEl.classList.add('failed');
                    // Remove animation class after animation completes
                    setTimeout(() => stepEl.classList.remove('failed'), 400);
                }
            } else {
                stepEl.classList.add('neutral');
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
    
    updateStatsDisplay() {
        const stats = this.gameEngine.getStats();
        
        // Update accuracy
        const accuracyEl = document.getElementById('accuracy-value');
        const accuracyFillEl = document.getElementById('accuracy-fill');
        if (accuracyEl) {
            accuracyEl.textContent = stats.accuracy;
            
            // Add animation class for changes
            if (this.gameEngine.lastInputAcc === 'success') {
                accuracyEl.parentElement.classList.add('animate-success');
                setTimeout(() => accuracyEl.parentElement.classList.remove('animate-success'), 500);
            } else if (this.gameEngine.lastInputAcc === 'fail') {
                accuracyEl.parentElement.classList.add('animate-error');
                setTimeout(() => accuracyEl.parentElement.classList.remove('animate-error'), 500);
            }
        }
        if (accuracyFillEl) {
            accuracyFillEl.style.width = `${stats.accuracy}%`;
        }
        
        // Update total inputs
        const totalInputsEl = document.getElementById('total-inputs');
        if (totalInputsEl) {
            totalInputsEl.textContent = stats.totalInputs;
        }
        
        // Update perfect inputs
        const perfectInputsEl = document.getElementById('perfect-inputs');
        if (perfectInputsEl) {
            perfectInputsEl.textContent = stats.perfectInputs;
        }
        
        // Update missed inputs
        const missedInputsEl = document.getElementById('missed-inputs');
        if (missedInputsEl) {
            missedInputsEl.textContent = stats.missedInputs;
        }
    }
    
    updateComboDisplay() {
        const comboDisplayEl = document.getElementById('combo-display');
        const comboCounterEl = document.getElementById('combo-counter');
        
        if (comboDisplayEl && comboCounterEl) {
            if (this.gameEngine.currentCombo > 1) {
                comboDisplayEl.classList.add('active');
                comboCounterEl.textContent = `×${this.gameEngine.currentCombo}`;
                
                // Add glow effect for high combos
                if (this.gameEngine.currentCombo >= 10) {
                    comboCounterEl.style.color = 'var(--perfect-purple)';
                } else if (this.gameEngine.currentCombo >= 5) {
                    comboCounterEl.style.color = 'var(--secondary-violet)';
                } else {
                    comboCounterEl.style.color = 'var(--combo-orange)';
                }
            } else {
                comboDisplayEl.classList.remove('active');
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
