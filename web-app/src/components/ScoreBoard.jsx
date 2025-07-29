import React, { useEffect, useState } from 'react';
import { Trophy, Target, Zap, Award } from 'lucide-react';

const StatCard = ({ icon: Icon, label, value, color = 'gold', animate = false }) => {
  const [isAnimating, setIsAnimating] = useState(false);

  useEffect(() => {
    if (animate) {
      setIsAnimating(true);
      const timer = setTimeout(() => setIsAnimating(false), 500);
      return () => clearTimeout(timer);
    }
  }, [animate, value]);

  return (
    <div className={`stat-card ${isAnimating ? 'glow-' + color : ''}`}>
      <div className="flex items-center justify-center mb-2">
        <Icon size={20} className={`text-${color}`} />
      </div>
      <div className={`stat-value text-${color}`}>
        {typeof value === 'number' ? value.toLocaleString() : value}
      </div>
      <div className="stat-label">{label}</div>
    </div>
  );
};

const ComboDisplay = ({ combo }) => {
  const [showCombo, setShowCombo] = useState(false);

  useEffect(() => {
    if (combo > 1) {
      setShowCombo(true);
    } else {
      setShowCombo(false);
    }
  }, [combo]);

  if (!showCombo) return null;

  return (
    <div className="mb-4">
      <div className="combo-display">
        ×{combo} COMBO
      </div>
    </div>
  );
};

const AccuracyBar = ({ accuracy }) => {
  const getAccuracyColor = (acc) => {
    if (acc >= 90) return 'gold-trophy';
    if (acc >= 75) return 'neon-cyan';
    if (acc >= 50) return 'combo-orange';
    return 'blood-red';
  };

  const getAccuracyText = (acc) => {
    if (acc >= 95) return 'PERFECT';
    if (acc >= 85) return 'EXCELLENT';
    if (acc >= 70) return 'GOOD';
    if (acc >= 50) return 'FAIR';
    return 'POOR';
  };

  return (
    <div className="accuracy-display">
      <div className="flex justify-between items-center mb-1">
        <span className="text-xs text-muted">ACCURACY</span>
        <span className={`text-xs font-bold text-${getAccuracyColor(accuracy)}`}>
          {getAccuracyText(accuracy)}
        </span>
      </div>
      <div className="w-full bg-gray-700 rounded-full h-2">
        <div
          className={`h-2 rounded-full bg-${getAccuracyColor(accuracy)} transition-all duration-300`}
          style={{ width: `${Math.min(accuracy, 100)}%` }}
        />
      </div>
      <div className="text-center mt-1">
        <span className={`text-sm font-mono text-${getAccuracyColor(accuracy)}`}>
          {accuracy.toFixed(1)}%
        </span>
      </div>
    </div>
  );
};

const ScoreBoard = ({ score, highScore, combo, accuracy }) => {
  const [prevScore, setPrevScore] = useState(score);
  const [prevHighScore, setPrevHighScore] = useState(highScore);
  const [scoreIncreased, setScoreIncreased] = useState(false);
  const [newHighScore, setNewHighScore] = useState(false);

  useEffect(() => {
    if (score > prevScore) {
      setScoreIncreased(true);
      const timer = setTimeout(() => setScoreIncreased(false), 500);
      setPrevScore(score);
      return () => clearTimeout(timer);
    }
  }, [score, prevScore]);

  useEffect(() => {
    if (highScore > prevHighScore) {
      setNewHighScore(true);
      const timer = setTimeout(() => setNewHighScore(false), 1000);
      setPrevHighScore(highScore);
      return () => clearTimeout(timer);
    }
  }, [highScore, prevHighScore]);

  return (
    <div className="scoreboard relative overflow-hidden">
      {/* Combo Display */}
      <ComboDisplay combo={combo} />

      {/* Main Stats */}
      <div className="grid gap-4">
        <StatCard
          icon={Zap}
          label="SCORE"
          value={score}
          color="gold"
          animate={scoreIncreased}
        />

        <StatCard
          icon={Trophy}
          label="BEST"
          value={highScore}
          color="cyan"
          animate={newHighScore}
        />

        {combo > 0 && (
          <StatCard
            icon={Award}
            label="COMBO"
            value={`×${combo}`}
            color="red"
            animate={combo > 1}
          />
        )}
      </div>

      {/* Accuracy Display */}
      {accuracy > 0 && (
        <div className="mt-4">
          <AccuracyBar accuracy={accuracy} />
        </div>
      )}

      {/* New High Score Celebration - Non-blocking overlay */}
      {newHighScore && (
        <div className="absolute top-2 right-2 z-10 pointer-events-none">
          <div className="text-center animate-bounce p-2 bg-black bg-opacity-75 rounded-lg" style={{ border: '1px solid var(--gold-trophy)' }}>
            <Trophy className="text-gold mx-auto mb-1" size={24} />
            <div className="text-xs font-bold text-gold whitespace-nowrap">NEW HIGH SCORE!</div>
          </div>
        </div>
      )}
    </div>
  );
};

export default ScoreBoard;
