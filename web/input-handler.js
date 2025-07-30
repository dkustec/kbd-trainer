/**
 * KBD Trainer Web - Input Handler
 * Handles keyboard and gamepad input with original logic
 */

class InputHandler {
    constructor(gameEngine) {
        this.gameEngine = gameEngine;
        this.lastDirection = 'neutral';
        this.gamepadIndex = -1;
        this.gamepadConnected = false;
        
        // Track gamepad input state to detect input changes
        this.gamepadCurrentDirection = 'neutral';
        this.gamepadLastProcessedDirection = 'neutral';
        
        // Track keyboard pressed keys for neutral detection
        this.pressedKeys = new Set();
        
        // Track button states to prevent repeat
        this.buttonStates = {
            start: false,
            back: false
        };
        
        this.init();
    }
    
    init() {
        // Keyboard event listeners
        document.addEventListener('keydown', (e) => this.handleKeydown(e));
        document.addEventListener('keyup', (e) => this.handleKeyup(e));
        
        // Gamepad connection events
        window.addEventListener('gamepadconnected', (e) => this.onGamepadConnected(e));
        window.addEventListener('gamepaddisconnected', (e) => this.onGamepadDisconnected(e));
        
        // Start gamepad polling if already connected
        this.checkGamepadConnection();
        if (this.gamepadConnected) {
            this.startGamepadPolling();
        }
        
        // Continuously check game state for miss pause timeout
        setInterval(() => {
            // During miss pause, keep calling update to check timeout
            if (this.gameEngine.inMissPause) {
                this.gameEngine.update(this.gameEngine.currInput || 'neutral');
            }
        }, 50); // Check every 50ms during miss pause
    }
    
    handleKeydown(event) {
        // Prevent key repeat
        if (event.repeat) return;
        
        // Block all input during miss pause
        if (this.gameEngine.inMissPause) {
            event.preventDefault();
            return;
        }
        
        // Track pressed directional keys
        if (this.isDirectionalKey(event.code)) {
            this.pressedKeys.add(event.code);
            this.updateKeyboardDirection();
            event.preventDefault();
            return;
        }
        
        // Handle special keys
        switch (event.code) {
            case 'Space':
                if (this.gameEngine.runGame) {
                    this.gameEngine.stopGame();
                } else {
                    this.gameEngine.startGame();
                }
                this.updateUI();
                event.preventDefault();
                break;
                
            case 'Escape':
                if (this.gameEngine.runGame) {
                    this.gameEngine.stopGame();
                    this.updateUI();
                }
                event.preventDefault();
                break;
                
            case 'KeyR':
                this.gameEngine.stopGame();
                this.gameEngine.playerPos = 0;
                this.gameEngine.score = 0;
                this.gameEngine.lastInputAcc = 'none';
                this.updateUI();
                event.preventDefault();
                break;
                
            case 'Digit1':
                this.gameEngine.setMode(0);
                this.updateUI();
                event.preventDefault();
                break;
            case 'Digit2':
                this.gameEngine.setMode(1);
                this.updateUI();
                event.preventDefault();
                break;
            case 'Digit3':
                this.gameEngine.setMode(2);
                this.updateUI();
                event.preventDefault();
                break;
            case 'Digit4':
                this.gameEngine.setMode(3);
                this.updateUI();
                event.preventDefault();
                break;
        }
    }
    
    handleKeyup(event) {
        // Block all input during miss pause
        if (this.gameEngine.inMissPause) {
            event.preventDefault();
            return;
        }
        
        // Track released directional keys
        if (this.isDirectionalKey(event.code)) {
            this.pressedKeys.delete(event.code);
            this.updateKeyboardDirection();
            event.preventDefault();
        }
    }
    
    isDirectionalKey(keyCode) {
        const directionalKeys = [
            // Arrow keys
            'ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight',
            // WASD
            'KeyW', 'KeyS', 'KeyA', 'KeyD',
            // Numpad
            'Numpad8', 'Numpad2', 'Numpad4', 'Numpad6',
            'Numpad7', 'Numpad9', 'Numpad1', 'Numpad3', 'Numpad5'
        ];
        return directionalKeys.includes(keyCode);
    }
    
    updateKeyboardDirection() {
        let direction = 'neutral';
        
        // Check for diagonal directions first (combinations)
        if ((this.pressedKeys.has('ArrowUp') || this.pressedKeys.has('KeyW') || this.pressedKeys.has('Numpad8')) &&
            (this.pressedKeys.has('ArrowLeft') || this.pressedKeys.has('KeyA') || this.pressedKeys.has('Numpad4'))) {
            direction = 'up-back';
        } else if ((this.pressedKeys.has('ArrowUp') || this.pressedKeys.has('KeyW') || this.pressedKeys.has('Numpad8')) &&
                   (this.pressedKeys.has('ArrowRight') || this.pressedKeys.has('KeyD') || this.pressedKeys.has('Numpad6'))) {
            direction = 'up-forward';
        } else if ((this.pressedKeys.has('ArrowDown') || this.pressedKeys.has('KeyS') || this.pressedKeys.has('Numpad2')) &&
                   (this.pressedKeys.has('ArrowLeft') || this.pressedKeys.has('KeyA') || this.pressedKeys.has('Numpad4'))) {
            direction = 'down-back';
        } else if ((this.pressedKeys.has('ArrowDown') || this.pressedKeys.has('KeyS') || this.pressedKeys.has('Numpad2')) &&
                   (this.pressedKeys.has('ArrowRight') || this.pressedKeys.has('KeyD') || this.pressedKeys.has('Numpad6'))) {
            direction = 'down-forward';
        }
        // Check for single directions
        else if (this.pressedKeys.has('ArrowUp') || this.pressedKeys.has('KeyW') || this.pressedKeys.has('Numpad8')) {
            direction = 'up';
        } else if (this.pressedKeys.has('ArrowDown') || this.pressedKeys.has('KeyS') || this.pressedKeys.has('Numpad2')) {
            direction = 'down';
        } else if (this.pressedKeys.has('ArrowLeft') || this.pressedKeys.has('KeyA') || this.pressedKeys.has('Numpad4')) {
            direction = 'back';
        } else if (this.pressedKeys.has('ArrowRight') || this.pressedKeys.has('KeyD') || this.pressedKeys.has('Numpad6')) {
            direction = 'forward';
        }
        // Check for numpad diagonals
        else if (this.pressedKeys.has('Numpad7')) {
            direction = 'up-back';
        } else if (this.pressedKeys.has('Numpad9')) {
            direction = 'up-forward';
        } else if (this.pressedKeys.has('Numpad1')) {
            direction = 'down-back';
        } else if (this.pressedKeys.has('Numpad3')) {
            direction = 'down-forward';
        } else if (this.pressedKeys.has('Numpad5')) {
            direction = 'neutral';
        }
        
        // Only process if direction changed
        if (direction !== this.lastDirection) {
            this.processDirectionInput(direction);
        }
    }

    onGamepadConnected(event) {
        console.log('Gamepad connected:', event.gamepad.id);
        console.log('Gamepad mapping:', event.gamepad.mapping);
        console.log('Axes count:', event.gamepad.axes.length);
        console.log('Buttons count:', event.gamepad.buttons.length);
        this.gamepadIndex = event.gamepad.index;
        this.gamepadConnected = true;
        this.startGamepadPolling();
    }
    
    onGamepadDisconnected(event) {
        console.log('Gamepad disconnected:', event.gamepad.id);
        if (event.gamepad.index === this.gamepadIndex) {
            this.gamepadConnected = false;
            this.gamepadIndex = -1;
        }
    }
    
    checkGamepadConnection() {
        const gamepads = navigator.getGamepads();
        for (let i = 0; i < gamepads.length; i++) {
            if (gamepads[i]) {
                this.gamepadIndex = i;
                this.gamepadConnected = true;
                console.log('Gamepad found:', gamepads[i].id);
                break;
            }
        }
    }
    
    startGamepadPolling() {
        const pollGamepad = () => {
            if (this.gamepadConnected) {
                this.handleGamepad();
                requestAnimationFrame(pollGamepad);
            }
        };
        requestAnimationFrame(pollGamepad);
    }
    
    handleGamepad() {
        // Block gamepad input during miss pause
        if (this.gameEngine.inMissPause) {
            return;
        }
        
        const gamepads = navigator.getGamepads();
        if (!gamepads[this.gamepadIndex]) return;
        
        const gamepad = gamepads[this.gamepadIndex];
        
        let direction = 'neutral';
        
        // Handle D-pad first (buttons 12-15)
        const dpadUp = gamepad.buttons[12] && gamepad.buttons[12].pressed;
        const dpadDown = gamepad.buttons[13] && gamepad.buttons[13].pressed;
        const dpadLeft = gamepad.buttons[14] && gamepad.buttons[14].pressed;
        const dpadRight = gamepad.buttons[15] && gamepad.buttons[15].pressed;
        
        if (dpadUp && dpadLeft) direction = 'up-back';
        else if (dpadUp && dpadRight) direction = 'up-forward';
        else if (dpadDown && dpadLeft) direction = 'down-back';
        else if (dpadDown && dpadRight) direction = 'down-forward';
        else if (dpadUp) direction = 'up';
        else if (dpadDown) direction = 'down';
        else if (dpadLeft) direction = 'back';
        else if (dpadRight) direction = 'forward';
        
        // If no D-pad input, check analog stick (axes 0 and 1)
        else {
            const stickX = gamepad.axes[0] || 0;
            const stickY = gamepad.axes[1] || 0;
            const deadzone = 0.3;
            
            // Check if stick is moved beyond deadzone
            if (Math.abs(stickX) > deadzone || Math.abs(stickY) > deadzone) {
                // Convert analog values to 8-way digital directions
                const angle = Math.atan2(stickY, stickX) * 180 / Math.PI;
                let normalizedAngle = (angle + 360) % 360;
                
                // Map angles to directions (0° = right/forward)
                if (normalizedAngle >= 337.5 || normalizedAngle < 22.5) {
                    direction = 'forward'; // Right
                } else if (normalizedAngle >= 22.5 && normalizedAngle < 67.5) {
                    direction = 'down-forward'; // Down-Right
                } else if (normalizedAngle >= 67.5 && normalizedAngle < 112.5) {
                    direction = 'down'; // Down
                } else if (normalizedAngle >= 112.5 && normalizedAngle < 157.5) {
                    direction = 'down-back'; // Down-Left
                } else if (normalizedAngle >= 157.5 && normalizedAngle < 202.5) {
                    direction = 'back'; // Left
                } else if (normalizedAngle >= 202.5 && normalizedAngle < 247.5) {
                    direction = 'up-back'; // Up-Left
                } else if (normalizedAngle >= 247.5 && normalizedAngle < 292.5) {
                    direction = 'up'; // Up
                } else if (normalizedAngle >= 292.5 && normalizedAngle < 337.5) {
                    direction = 'up-forward'; // Up-Right
                }
                
                console.log(`Stick: X=${stickX.toFixed(2)}, Y=${stickY.toFixed(2)}, Angle=${normalizedAngle.toFixed(1)}°, Direction=${direction}`);
            }
        }
        
        // Process direction - only when direction actually changes
        if (direction !== this.gamepadCurrentDirection) {
            console.log(`Gamepad direction changed: ${this.gamepadCurrentDirection} → ${direction}`);
            this.gamepadCurrentDirection = direction;
            this.processDirectionInput(direction);
        }
        
        // Handle face buttons
        const startPressed = gamepad.buttons[0] && gamepad.buttons[0].pressed; // A button
        const backPressed = gamepad.buttons[1] && gamepad.buttons[1].pressed; // B button
        
        // Block gamepad special buttons during miss pause
        if (!this.gameEngine.inMissPause) {
            if (startPressed && !this.buttonStates.start) {
                if (this.gameEngine.runGame) {
                    this.gameEngine.stopGame();
                } else {
                    this.gameEngine.startGame();
                }
                this.updateUI();
            }
            
            if (backPressed && !this.buttonStates.back) {
                if (this.gameEngine.runGame) {
                    this.gameEngine.stopGame();
                    this.updateUI();
                }
            }
        }
        
        this.buttonStates.start = startPressed;
        this.buttonStates.back = backPressed;
    }
    
    processDirectionInput(direction) {
        // Update last direction for keyboard compatibility
        this.lastDirection = direction;
        
        // Send input to game engine
        this.gameEngine.update(direction);
        
        // Immediate UI update for responsive highlighting
        this.updateUI();
    }
    
    // Reset input state - called when game resets
    resetInputState() {
        this.lastDirection = 'neutral';
        this.gamepadCurrentDirection = 'neutral';
        this.pressedKeys.clear(); // Clear all pressed keys
    }
    
    updateUI() {
        // Trigger UI update
        if (window.uiController) {
            window.uiController.updateUI();
        }
    }
}
