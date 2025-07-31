/**
 * KBD Trainer Web - UI Controller
 * Simple, focused UI matching original design priorities
 */

class UIController {
    constructor(gameEngine) {
        this.gameEngine = gameEngine;
        this.hasStartedGame = false; // Track if game has been started at least once
        this.init();
    }
    
    init() {
        this.setupEventListeners();
        this.updateUI();
        
        // Reduce update frequency to prevent flashing
        setInterval(() => {
            this.updateUI();
        }, 250); // Update every 250ms instead of 100ms
        
        // Update gamepad status every 500ms
        setInterval(() => {
            this.updateGamepadPanel();
        }, 500);
        
        // Expose globally
        window.uiController = this;
        // Initialize instructions toggle UI
        this.initializeInstructionsToggle();
    }
    
    setupEventListeners() {
        // Enhanced mode selection cards
        document.querySelectorAll('.mode-card').forEach((card, index) => {
            card.addEventListener('click', () => {
                this.selectMode(index);
            });
        });
        
        // Control buttons
        document.getElementById('start-btn').addEventListener('click', () => this.toggleGame());
        document.getElementById('reset-btn').addEventListener('click', () => this.resetGame());
        
        // Gamepad panel toggle
        document.getElementById('gamepad-toggle').addEventListener('click', () => this.toggleGamepadPanel());
        
        // Mode selector toggle for mobile
        const modeSelectorToggle = document.getElementById('mode-selector-toggle');
        if (modeSelectorToggle) {
            modeSelectorToggle.addEventListener('click', () => this.toggleModeSelector());
        }
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
        
        // Auto-collapse mode selector on mobile after selection
        if (window.innerWidth <= 480) {
            const modeSelector = document.getElementById('mode-selector');
            const toggleBtn = document.getElementById('mode-selector-toggle');
            if (modeSelector && !modeSelector.classList.contains('collapsed')) {
                setTimeout(() => {
                    modeSelector.classList.add('collapsed');
                    toggleBtn.textContent = 'Show';
                }, 300); // Small delay to show the selection
            }
        }
    }
    
    toggleGame() {
        if (this.gameEngine.runGame) {
            this.gameEngine.stopGame();
        } else {
            this.gameEngine.startGame();
            this.hasStartedGame = true; // Mark that game has been started
        }
        this.updateUI();
    }
    
    resetGame() {
        this.gameEngine.resetGame(); // Use enhanced reset method
        this.updateUI();
    }
    
    updateUI() {
        // Always update game state first (for miss pause countdown, etc.)
        this.gameEngine.updateGameState();
        
        // Update mode selection
        this.updateModeButtons();
        
        // Update game display
        this.updateGameDisplay();
        
        // Update controls
        this.updateControls();
    }
    
    updateModeButtons() {
        // Update mode cards selection (not buttons anymore)
        const modeCards = document.querySelectorAll('.mode-card');
        modeCards.forEach((card, index) => {
            if (index === this.gameEngine.selectedMode) {
                card.classList.add('selected');
            } else {
                card.classList.remove('selected');
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
        
        // Only rebuild if pattern length changed or pattern is empty
        if (patternEl.children.length !== pattern.length) {
            patternEl.innerHTML = '';
            
            pattern.forEach((direction, index) => {
                const stepEl = document.createElement('div');
                stepEl.className = 'pattern-step';
                
                // Create and append direction image
                const directionImg = this.createDirectionImage(direction, 'pattern-direction');
                stepEl.appendChild(directionImg);
                
                stepEl.dataset.index = index; // Store index for reference
                patternEl.appendChild(stepEl);
            });
        } else {
            // Update images without rebuilding if pattern content changed
            Array.from(patternEl.children).forEach((stepEl, index) => {
                const expectedDirection = pattern[index];
                const currentImg = stepEl.querySelector('.pattern-direction');
                const expectedSrc = this.directionToImage(expectedDirection);
                
                if (currentImg.src !== expectedSrc) {
                    currentImg.src = expectedSrc;
                    currentImg.alt = expectedDirection;
                }
            });
        }
        
        // Update states of existing pattern steps
        Array.from(patternEl.children).forEach((stepEl, index) => {
            // Clear all state classes first
            stepEl.className = 'pattern-step';
            
            // Determine state based on game progress
            if (this.gameEngine.runGame) {
                if (index === this.gameEngine.playerPos) {
                    stepEl.classList.add('current');
                } else if (index < this.gameEngine.playerPos) {
                    stepEl.classList.add('completed');
                } else {
                    stepEl.classList.add('neutral');
                }
            } else {
                stepEl.classList.add('neutral');
            }
        });
        
        // Handle failure animation separately and only if game has actually started
        if (this.gameEngine.lastInputAcc === 'fail' && 
            this.gameEngine.runGame && 
            this.hasStartedGame &&
            this.gameEngine.totalInputs > 0) {
            const currentIcon = patternEl.children[this.gameEngine.playerPos];
            if (currentIcon && !currentIcon.classList.contains('failed')) {
                currentIcon.classList.add('failed');
                setTimeout(() => {
                    if (currentIcon) currentIcon.classList.remove('failed');
                }, 400);
            }
        }
    }
    
    updateStatusDisplay() {
        const statusEl = document.getElementById('game-status');
        if (!statusEl) return;
        
        const status = this.gameEngine.getStatus();
        
        switch (status) {
            case 'menu':
                statusEl.textContent = 'Select mode and press Start';
                statusEl.className = 'game-status status-menu';
                statusEl.style.flexDirection = 'row'; // Ensure proper layout
                break;
            case 'playing':
                statusEl.textContent = 'Training Active';
                statusEl.className = 'game-status status-playing';
                statusEl.style.flexDirection = 'row';
                
                // Update the separate Next input display
                const nextDisplayEl = document.getElementById('next-input-display');
                if (nextDisplayEl) {
                    const expected = this.gameEngine.getExpectedInput();
                    nextDisplayEl.innerHTML = '';
                    const directionImg = this.createDirectionImage(expected);
                    directionImg.style.width = '36px';
                    directionImg.style.height = '36px';
                    nextDisplayEl.appendChild(directionImg);
                }
                break;
                break;
            case 'miss_pause':
                // Display countdown timer with proper structure for flexbox
                const countdown = Math.ceil(this.gameEngine.missCountdown);
                statusEl.innerHTML = ''; // Clear previous content
                
                const missText = document.createElement('div');
                missText.textContent = 'MISS!';
                missText.style.fontSize = '2.2rem';
                missText.style.fontWeight = 'bold';
                missText.style.color = 'var(--error-red)';
                
                const countdownText = document.createElement('div');
                countdownText.textContent = `Start again in ${countdown}...`;
                countdownText.className = 'countdown';
                countdownText.style.fontSize = '1.4rem';
                countdownText.style.marginTop = '8px';
                
                statusEl.appendChild(missText);
                statusEl.appendChild(countdownText);
                statusEl.className = 'game-status status-miss';
                statusEl.style.flexDirection = 'column';
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
            // Clear existing content
            lastInputEl.innerHTML = '';
            
            // Create and append direction image
            const directionImg = this.createDirectionImage(this.gameEngine.lastInput, 'last-input-direction');
            lastInputEl.appendChild(directionImg);
            
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
    
    // New function to get direction image
    directionToImage(direction) {
        const imageMap = {
            'neutral': 'n.png',
            'up': 'u.png',
            'up-forward': 'uf.png',
            'forward': 'f.png',
            'down-forward': 'df.png',
            'down': 'd.png',
            'down-back': 'db.png',
            'back': 'b.png',
            'up-back': 'ub.png'
        };
        
        const filename = imageMap[direction] || 'n.png';
        return `assets/directions/${filename}`;
    }
    
    // Create direction image element
    createDirectionImage(direction, className = '') {
        const img = document.createElement('img');
        img.src = this.directionToImage(direction);
        img.alt = direction;
        img.className = `direction-icon ${className}`;
        img.loading = 'lazy';
        
        // Add error handling for missing images
        img.onerror = () => {
            console.warn(`Failed to load direction image: ${img.src}`);
            // Fallback to colored text symbol with better styling
            const span = document.createElement('span');
            span.textContent = this.directionToSymbol(direction);
            span.className = img.className;
            span.style.fontSize = '1.5rem';
            span.style.color = 'var(--accent-color)';
            span.style.fontWeight = 'bold';
            span.style.textShadow = '0 0 4px rgba(139, 92, 246, 0.5)';
            img.parentNode?.replaceChild(span, img);
        };
        
        return img;
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
    
    toggleModeSelector() {
        const modeSelector = document.getElementById('mode-selector');
        const toggleBtn = document.getElementById('mode-selector-toggle');
        
        if (modeSelector.classList.contains('collapsed')) {
            modeSelector.classList.remove('collapsed');
            toggleBtn.textContent = 'Hide';
        } else {
            modeSelector.classList.add('collapsed');
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
    
    /**
     * Initialize the instructions panel toggle behavior.
     */
    initializeInstructionsToggle() {
        const toggle = document.getElementById('instructions-toggle');
        const content = document.getElementById('instructions-content');
        if (toggle && content) {
            toggle.addEventListener('click', () => {
                const hidden = content.classList.contains('hidden');
                if (hidden) {
                    content.classList.remove('hidden');
                    toggle.textContent = 'Hide';
                    setTimeout(() => {
                        window.scrollTo({ top: document.body.scrollHeight, behavior: 'smooth' });
                    }, 100);
                } else {
                    content.classList.add('hidden');
                    toggle.textContent = 'Show';
                }
            });
        }
    }
}
