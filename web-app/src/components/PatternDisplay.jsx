import React, { useEffect, useState } from 'react';
import { DIRECTION_SYMBOLS, INPUT_ACCURACY } from '../gameData.js';

const DirectionIcon = ({ direction, state, size = 'large' }) => {
  const [animate, setAnimate] = useState('');

  useEffect(() => {
    if (state === 'completed') {
      setAnimate('success-flash');
      const timer = setTimeout(() => setAnimate(''), 500);
      return () => clearTimeout(timer);
    } else if (state === 'failed') {
      setAnimate('damage-shake');
      const timer = setTimeout(() => setAnimate(''), 400);
      return () => clearTimeout(timer);
    }
  }, [state]);

  const getIconClasses = () => {
    let classes = `direction-icon ${size}`;

    switch (state) {
      case 'completed':
        classes += ' completed';
        break;
      case 'active':
        classes += ' active';
        break;
      case 'failed':
        classes += ' failed';
        break;
      case 'neutral':
        classes += ' neutral';
        break;
      default:
        break;
    }

    if (animate) {
      classes += ` ${animate}`;
    }

    return classes;
  };

  return (
    <div className={getIconClasses()}>
      <span className="icon-symbol">
        {DIRECTION_SYMBOLS[direction] || '○'}
      </span>
      {state === 'active' && <div className="pulse-ring" />}
    </div>
  );
};

const PatternDisplay = ({
  pattern,
  currentPosition,
  lastInputAccuracy,
  inGame
}) => {
  const [flashEffect, setFlashEffect] = useState('');

  useEffect(() => {
    if (lastInputAccuracy === INPUT_ACCURACY.SUCCESS) {
      setFlashEffect('perfect-input');
      const timer = setTimeout(() => setFlashEffect(''), 500);
      return () => clearTimeout(timer);
    } else if (lastInputAccuracy === INPUT_ACCURACY.FAIL) {
      setFlashEffect('combo-break');
      const timer = setTimeout(() => setFlashEffect(''), 800);
      return () => clearTimeout(timer);
    }
  }, [lastInputAccuracy]);

  const getIconState = (index) => {
    if (!inGame) return 'neutral';

    if (index < currentPosition) {
      return 'completed';
    } else if (index === currentPosition) {
      return lastInputAccuracy === INPUT_ACCURACY.FAIL ? 'failed' : 'active';
    }
    return 'neutral';
  };

  return (
    <div className={`pattern-display ${flashEffect}`}>
      {pattern.map((direction, index) => (
        <DirectionIcon
          key={`${direction}-${index}`}
          direction={direction}
          state={getIconState(index)}
          size="large"
        />
      ))}

      {/* Screen flash effect for special events */}
      {lastInputAccuracy === INPUT_ACCURACY.SUCCESS && currentPosition === 0 && (
        <div className="screen-flash" />
      )}
    </div>
  );
};

export default PatternDisplay;
