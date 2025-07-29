import React from 'react';
import { DIRECTION_SYMBOLS } from '../gameData.js';
import { Star, Lock, CheckCircle } from 'lucide-react';

const DifficultyBadge = ({ difficulty }) => {
  const getDifficultyConfig = (diff) => {
    switch (diff) {
      case 'BEGINNER':
        return { color: 'green', icon: CheckCircle, stars: 1 };
      case 'INTERMEDIATE':
        return { color: 'yellow', icon: Star, stars: 2 };
      case 'ADVANCED':
        return { color: 'red', icon: Lock, stars: 3 };
      default:
        return { color: 'gray', icon: CheckCircle, stars: 1 };
    }
  };

  const config = getDifficultyConfig(difficulty);
  const Icon = config.icon;

  return (
    <div className="flex items-center gap-1 text-xs">
      <Icon size={12} />
      <span>{difficulty}</span>
      <div className="flex">
        {Array.from({ length: 3 }, (_, i) => (
          <Star
            key={i}
            size={8}
            className={i < config.stars ? 'text-yellow-400 fill-current' : 'text-gray-600'}
          />
        ))}
      </div>
    </div>
  );
};

const PatternPreview = ({ pattern, size = 'small' }) => {
  return (
    <div className="pattern-preview flex gap-1 justify-center">
      {pattern.slice(0, 4).map((direction, index) => (
        <div
          key={index}
          className={`pattern-preview-icon ${size}`}
        >
          {DIRECTION_SYMBOLS[direction] || '○'}
        </div>
      ))}
      {pattern.length > 4 && (
        <div className={`pattern-preview-icon ${size} text-muted`}>
          ...
        </div>
      )}
    </div>
  );
};

const ModeCard = ({ mode, isSelected, highScore, disabled, onClick }) => {
  const handleClick = () => {
    if (!disabled && onClick) {
      onClick(mode.id);
    }
  };

  const hasHighScore = highScore && highScore > 0;

  return (
    <div
      className={`mode-card ${isSelected ? 'active' : ''} ${disabled ? 'opacity-50 cursor-not-allowed' : 'cursor-pointer'}`}
      onClick={handleClick}
      style={{
        borderColor: isSelected ? mode.color : undefined
      }}
    >
      {/* Mode Title */}
      <div className="mode-title">{mode.name}</div>

      {/* Difficulty Badge */}
      <div className="mode-difficulty mb-2">
        <DifficultyBadge difficulty={mode.difficulty} />
      </div>

      {/* Pattern Preview */}
      <PatternPreview pattern={mode.pattern} />

      {/* High Score */}
      {hasHighScore && (
        <div className="mt-2 pt-2 border-t border-gray-600">
          <div className="text-xs text-muted">BEST</div>
          <div className="text-sm font-bold text-gold">
            {highScore.toLocaleString()}
          </div>
        </div>
      )}

      {/* Selection Indicator */}
      {isSelected && (
        <div className="absolute top-2 right-2">
          <div className="w-2 h-2 rounded-full bg-gold-trophy animate-pulse" />
        </div>
      )}

      {/* Mode Description Tooltip */}
      <div className="mode-description text-xs text-muted mt-1 text-center">
        {mode.description}
      </div>
    </div>
  );
};

const ModeSelector = ({ modes, selectedMode, highScores, disabled, onModeSelect }) => {
  return (
    <div className="mode-selector">
      {/* Header */}
      <div className="text-center mb-4">
        <h3 className="text-lg font-bold text-gold mb-1">TRAINING MODES</h3>
        <p className="text-xs text-muted">
          {disabled ? 'Game in Progress' : 'Select a mode to practice'}
        </p>
      </div>

      {/* Mode Grid */}
      <div className="grid grid-cols-2 gap-3">
        {modes.map((mode) => (
          <ModeCard
            key={mode.id}
            mode={mode}
            isSelected={selectedMode === mode.id}
            highScore={highScores[mode.id]}
            disabled={disabled}
            onClick={onModeSelect}
          />
        ))}
      </div>

      {/* Controls Hint */}
      {!disabled && (
        <div className="text-center mt-4 pt-4 border-t border-gray-700">
          <p className="text-xs text-muted">
            Use ◄► to navigate • A/Enter to select
          </p>
        </div>
      )}

      {/* Statistics Summary */}
      <div className="mt-4 pt-4 border-t border-gray-700">
        <div className="text-center">
          <div className="text-xs text-muted mb-1">TOTAL PROGRESS</div>
          <div className="flex justify-center gap-4 text-xs">
            <div>
              <span className="text-gold">{Object.keys(highScores).length}</span>
              <span className="text-muted">/{modes.length} completed</span>
            </div>
            <div>
              <span className="text-cyan">
                {Object.values(highScores).reduce((sum, score) => sum + score, 0).toLocaleString()}
              </span>
              <span className="text-muted"> total score</span>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default ModeSelector;
