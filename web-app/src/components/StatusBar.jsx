import React, { useState, useEffect } from 'react';
import { Gamepad2, Clock, Wifi, WifiOff, Settings } from 'lucide-react';

const ControllerStatus = ({ connected, id }) => {
  const [pulse, setPulse] = useState(false);

  useEffect(() => {
    if (connected) {
      setPulse(true);
      const timer = setTimeout(() => setPulse(false), 500);
      return () => clearTimeout(timer);
    }
  }, [connected]);

  const getControllerName = (gamepadId) => {
    if (!gamepadId) return 'No Controller';

    // Simplify common controller names
    if (gamepadId.includes('Xbox')) return 'Xbox Controller';
    if (gamepadId.includes('PlayStation')) return 'PlayStation Controller';
    if (gamepadId.includes('DualShock')) return 'DualShock Controller';
    if (gamepadId.includes('DualSense')) return 'DualSense Controller';

    // Truncate long names
    return gamepadId.length > 20 ? gamepadId.substring(0, 20) + '...' : gamepadId;
  };

  return (
    <div className={`controller-status ${connected ? 'connected' : 'disconnected'} ${pulse ? 'animate-pulse' : ''}`}>
      <Gamepad2 size={16} />
      <span className="ml-2">
        {connected ? getControllerName(id) : 'No Controller'}
      </span>
      {connected ? (
        <Wifi size={12} className="ml-1 text-green-400" />
      ) : (
        <WifiOff size={12} className="ml-1 text-red-400" />
      )}
    </div>
  );
};

const GameTimer = ({ gameTime, isRunning }) => {
  const [displayTime, setDisplayTime] = useState(0);

  useEffect(() => {
    if (isRunning) {
      const interval = setInterval(() => {
        setDisplayTime(gameTime);
      }, 100);
      return () => clearInterval(interval);
    } else {
      setDisplayTime(0);
    }
  }, [gameTime, isRunning]);

  const formatTime = (ms) => {
    const seconds = Math.floor(ms / 1000);
    const minutes = Math.floor(seconds / 60);
    const remainingSeconds = seconds % 60;
    const centiseconds = Math.floor((ms % 1000) / 10);

    if (minutes > 0) {
      return `${minutes}:${remainingSeconds.toString().padStart(2, '0')}.${centiseconds.toString().padStart(2, '0')}`;
    }
    return `${remainingSeconds}.${centiseconds.toString().padStart(2, '0')}s`;
  };

  if (!isRunning) return null;

  return (
    <div className="timer-display flex items-center">
      <Clock size={16} className="mr-2" />
      <span className="font-mono">{formatTime(displayTime)}</span>
    </div>
  );
};

const InputHints = ({ gamepadConnected }) => {
  const hints = gamepadConnected
    ? [
      { key: 'D-Pad/Stick', action: 'Move' },
      { key: 'A', action: 'Select' },
      { key: 'B', action: 'Back' },
      { key: 'Start', action: 'Pause' }
    ]
    : [
      { key: '←→↑↓', action: 'Move' },
      { key: 'Enter', action: 'Select' },
      { key: 'Esc', action: 'Back' },
      { key: 'Space', action: 'Pause' }
    ];

  return (
    <div className="input-hints flex items-center gap-4 text-xs text-muted">
      {hints.map((hint, index) => (
        <div key={index} className="flex items-center gap-1">
          <kbd className="px-1 py-0.5 bg-gray-700 rounded text-xs">{hint.key}</kbd>
          <span>{hint.action}</span>
        </div>
      ))}
    </div>
  );
};

const PerformanceStats = ({ fps, latency, visible }) => {
  if (!visible) return null;

  return (
    <div className="performance-stats text-xs text-muted">
      <div className="flex gap-3">
        <span>FPS: {fps}</span>
        <span>Latency: {latency}ms</span>
      </div>
    </div>
  );
};

const StatusBar = ({
  gamepadConnected,
  gamepadId,
  gameTime,
  isGameRunning = false,
  showPerformanceStats = false,
  onSettingsClick
}) => {
  const [currentTime, setCurrentTime] = useState(new Date());
  const [fps, setFps] = useState(60);
  const [latency, setLatency] = useState(0);

  // Update current time every second
  useEffect(() => {
    const interval = setInterval(() => {
      setCurrentTime(new Date());
    }, 1000);
    return () => clearInterval(interval);
  }, []);

  // Simple FPS monitoring
  useEffect(() => {
    let lastTime = performance.now();
    let frames = 0;

    const countFrames = () => {
      frames++;
      const currentTime = performance.now();

      if (currentTime - lastTime >= 1000) {
        setFps(Math.round((frames * 1000) / (currentTime - lastTime)));
        frames = 0;
        lastTime = currentTime;
      }

      requestAnimationFrame(countFrames);
    };

    const animationFrame = requestAnimationFrame(countFrames);
    return () => cancelAnimationFrame(animationFrame);
  }, []);

  // Simulated latency monitoring (in a real app, this would measure actual input lag)
  useEffect(() => {
    const interval = setInterval(() => {
      setLatency(Math.floor(Math.random() * 5) + 12); // Simulate 12-17ms latency
    }, 2000);
    return () => clearInterval(interval);
  }, []);

  return (
    <>
      {/* Main Status Bar Content */}
      <div className="status-bar-main">
        {/* Left Section - Controller Status */}
        <div className="flex items-center gap-4">
          <ControllerStatus connected={gamepadConnected} id={gamepadId} />
          {showPerformanceStats && (
            <PerformanceStats fps={fps} latency={latency} visible={showPerformanceStats} />
          )}
        </div>

        {/* Center Section - Timer (only when game is running) */}
        {isGameRunning && (
          <div className="flex items-center justify-center">
            <GameTimer gameTime={gameTime} isRunning={isGameRunning} />
          </div>
        )}

        {/* Right Section - System Time & Settings */}
        <div className="flex items-center gap-4">
          <div className="system-time text-sm text-muted font-mono">
            {currentTime.toLocaleTimeString([], {
              hour: '2-digit',
              minute: '2-digit',
              hour12: false
            })}
          </div>

          {onSettingsClick && (
            <button
              onClick={onSettingsClick}
              className="settings-button p-1 rounded hover:bg-gray-700 transition-colors"
              title="Settings"
            >
              <Settings size={16} />
            </button>
          )}
        </div>
      </div>

      {/* Input Hints (only show when not in game) */}
      {!isGameRunning && (
        <div className="input-hints-container text-center">
          <InputHints gamepadConnected={gamepadConnected} />
        </div>
      )}
    </>
  );
};

export default StatusBar;
