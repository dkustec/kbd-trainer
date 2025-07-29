import { useState, useEffect, useCallback } from 'react';
import { GAME_DIRECTIONS, GAMEPAD_MAPPING } from '../gameData.js';

export const useGamepad = () => {
  const [gamepadState, setGamepadState] = useState({
    connected: false,
    index: -1,
    id: '',
    direction: GAME_DIRECTIONS.NEUTRAL,
    selectPressed: false,
    backPressed: false,
    startPressed: false
  });

  const [lastInputTime, setLastInputTime] = useState(0);
  const INPUT_DEBOUNCE = 100; // ms

  // Map gamepad inputs to direction (handles both button D-pad and analog stick)
  const mapInputsToDirection = useCallback((leftX, leftY, dpadButtons, dpadAxes) => {
    const deadzone = 0.3;

    // Check D-pad buttons first (DualSense uses buttons for D-pad)
    if (dpadButtons) {
      const { up, down, left, right } = dpadButtons;

      // Handle button combinations for diagonals
      if (up && right) return GAME_DIRECTIONS.UP_FORWARD;
      if (up && left) return GAME_DIRECTIONS.UP_BACK;
      if (down && right) return GAME_DIRECTIONS.DOWN_FORWARD;
      if (down && left) return GAME_DIRECTIONS.DOWN_BACK;

      // Handle single directions
      if (up) return GAME_DIRECTIONS.UP;
      if (down) return GAME_DIRECTIONS.DOWN;
      if (right) return GAME_DIRECTIONS.FORWARD;
      if (left) return GAME_DIRECTIONS.BACK;
    }

    // Check D-pad axes (some controllers map D-pad to axes)
    if (dpadAxes && (Math.abs(dpadAxes.x) > deadzone || Math.abs(dpadAxes.y) > deadzone)) {
      const dpadX = dpadAxes.x;
      const dpadY = dpadAxes.y;

      if (dpadY < -deadzone && Math.abs(dpadX) < deadzone) return GAME_DIRECTIONS.UP;
      if (dpadY > deadzone && Math.abs(dpadX) < deadzone) return GAME_DIRECTIONS.DOWN;
      if (dpadX > deadzone && Math.abs(dpadY) < deadzone) return GAME_DIRECTIONS.FORWARD;
      if (dpadX < -deadzone && Math.abs(dpadY) < deadzone) return GAME_DIRECTIONS.BACK;

      // Diagonal directions
      if (dpadY < -deadzone && dpadX > deadzone) return GAME_DIRECTIONS.UP_FORWARD;
      if (dpadY < -deadzone && dpadX < -deadzone) return GAME_DIRECTIONS.UP_BACK;
      if (dpadY > deadzone && dpadX > deadzone) return GAME_DIRECTIONS.DOWN_FORWARD;
      if (dpadY > deadzone && dpadX < -deadzone) return GAME_DIRECTIONS.DOWN_BACK;
    }

    // Fall back to analog stick
    if (Math.abs(leftX) > deadzone || Math.abs(leftY) > deadzone) {
      if (leftY < -deadzone && Math.abs(leftX) < deadzone) return GAME_DIRECTIONS.UP;
      if (leftY > deadzone && Math.abs(leftX) < deadzone) return GAME_DIRECTIONS.DOWN;
      if (leftX > deadzone && Math.abs(leftY) < deadzone) return GAME_DIRECTIONS.FORWARD;
      if (leftX < -deadzone && Math.abs(leftY) < deadzone) return GAME_DIRECTIONS.BACK;

      // Diagonal directions
      if (leftY < -deadzone && leftX > deadzone) return GAME_DIRECTIONS.UP_FORWARD;
      if (leftY < -deadzone && leftX < -deadzone) return GAME_DIRECTIONS.UP_BACK;
      if (leftY > deadzone && leftX > deadzone) return GAME_DIRECTIONS.DOWN_FORWARD;
      if (leftY > deadzone && leftX < -deadzone) return GAME_DIRECTIONS.DOWN_BACK;
    }

    return GAME_DIRECTIONS.NEUTRAL;
  }, []);

  // Poll gamepad state
  const pollGamepad = useCallback(() => {
    const gamepads = navigator.getGamepads();
    const currentTime = performance.now();

    if (gamepadState.index >= 0 && gamepads[gamepadState.index]) {
      const gamepad = gamepads[gamepadState.index];

      // Get direction from analog stick
      const leftX = gamepad.axes[GAMEPAD_MAPPING.AXES.LEFT_X] || 0;
      const leftY = gamepad.axes[GAMEPAD_MAPPING.AXES.LEFT_Y] || 0;

      // Get D-pad button states (DualSense/DualShock uses buttons 12-15 for D-pad)
      const dpadButtons = {
        up: gamepad.buttons[12]?.pressed || false,     // D-pad Up
        down: gamepad.buttons[13]?.pressed || false,   // D-pad Down
        left: gamepad.buttons[14]?.pressed || false,   // D-pad Left
        right: gamepad.buttons[15]?.pressed || false   // D-pad Right
      };

      // Some controllers use axes for D-pad (fallback)
      const dpadAxes = {
        x: gamepad.axes[6] || 0, // D-pad X on some controllers
        y: gamepad.axes[7] || 0  // D-pad Y on some controllers
      };

      const direction = mapInputsToDirection(leftX, leftY, dpadButtons, dpadAxes);

      // Get button states with debouncing
      const selectPressed = gamepad.buttons[GAMEPAD_MAPPING.BUTTONS.A]?.pressed || false;
      const backPressed = gamepad.buttons[GAMEPAD_MAPPING.BUTTONS.B]?.pressed || false;
      const startPressed = gamepad.buttons[GAMEPAD_MAPPING.BUTTONS.START]?.pressed || false;

      // Only update if enough time has passed (debouncing)
      if (currentTime - lastInputTime > INPUT_DEBOUNCE) {
        const newState = {
          ...gamepadState,
          direction,
          selectPressed,
          backPressed,
          startPressed
        };

        // Check if state actually changed
        const stateChanged =
          newState.direction !== gamepadState.direction ||
          newState.selectPressed !== gamepadState.selectPressed ||
          newState.backPressed !== gamepadState.backPressed ||
          newState.startPressed !== gamepadState.startPressed;

        if (stateChanged) {
          setGamepadState(newState);
          setLastInputTime(currentTime);
        }
      }
    }
  }, [gamepadState, lastInputTime, mapInputsToDirection]);

  // Handle gamepad connection
  useEffect(() => {
    const handleGamepadConnected = (e) => {
      console.log('Gamepad connected:', e.gamepad.id);
      setGamepadState(prev => ({
        ...prev,
        connected: true,
        index: e.gamepad.index,
        id: e.gamepad.id
      }));
    };

    const handleGamepadDisconnected = (e) => {
      console.log('Gamepad disconnected:', e.gamepad.id);
      setGamepadState(prev => ({
        ...prev,
        connected: false,
        index: -1,
        id: '',
        direction: GAME_DIRECTIONS.NEUTRAL,
        selectPressed: false,
        backPressed: false,
        startPressed: false
      }));
    };

    window.addEventListener('gamepadconnected', handleGamepadConnected);
    window.addEventListener('gamepaddisconnected', handleGamepadDisconnected);

    return () => {
      window.removeEventListener('gamepadconnected', handleGamepadConnected);
      window.removeEventListener('gamepaddisconnected', handleGamepadDisconnected);
    };
  }, []);

  // Set up polling interval
  useEffect(() => {
    let animationFrame;

    if (gamepadState.connected) {
      const poll = () => {
        pollGamepad();
        animationFrame = requestAnimationFrame(poll);
      };
      animationFrame = requestAnimationFrame(poll);
    }

    return () => {
      if (animationFrame) {
        cancelAnimationFrame(animationFrame);
      }
    };
  }, [gamepadState.connected, pollGamepad]);

  // Check for already connected gamepads on mount
  useEffect(() => {
    const checkExistingGamepads = () => {
      const gamepads = navigator.getGamepads();
      for (let i = 0; i < gamepads.length; i++) {
        if (gamepads[i]) {
          setGamepadState(prev => ({
            ...prev,
            connected: true,
            index: i,
            id: gamepads[i].id
          }));
          break;
        }
      }
    };

    // Small delay to ensure browser has initialized gamepads
    const timeout = setTimeout(checkExistingGamepads, 100);
    return () => clearTimeout(timeout);
  }, []);

  return gamepadState;
};

export default useGamepad;
