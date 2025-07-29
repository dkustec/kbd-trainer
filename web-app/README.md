# KBD Trainer Web App

A modern, tournament-style web application for practicing fighting game keyboard inputs. Built with React and featuring a sleek dark theme with gold/cyan accents.

## 🎮 Features

- **Tournament Design**: Professional fighting game tournament aesthetic
- **Controller Support**: Full gamepad/controller integration via WebGamepad API
- **Multiple Training Modes**:
  - P1/P2 Korean Back Dash (KBD)
  - P1/P2 Wave Dash (WD)
- **Real-time Feedback**: Visual and audio feedback for inputs
- **Progress Tracking**: High scores, accuracy, combo tracking
- **Responsive Design**: Works on desktop, tablet, and mobile
- **PWA Support**: Install as a native app

## 🎯 Training Modes

### Korean Back Dash (KBD)
- **P1 KBD**: ◄ ○ ◄ ◣ (Back → Neutral → Back → Down-Back)
- **P2 KBD**: ► ○ ► ◢ (Forward → Neutral → Forward → Down-Forward)

### Wave Dash (WD)
- **P1 WD**: ► ○ ▼ ◢ ► ○ (Forward → Neutral → Down → Down-Forward → Forward → Neutral)
- **P2 WD**: ◄ ○ ▼ ◣ ◄ ○ (Back → Neutral → Down → Down-Back → Back → Neutral)

## 🕹️ Controls

### Controller (Recommended)
- **D-Pad/Left Stick**: Navigate directions
- **A Button**: Select/Start
- **B Button**: Back/Exit
- **Start Button**: Pause

### Keyboard Fallback
- **Arrow Keys / WASD**: Navigate directions
- **Enter/Space**: Select/Start
- **Escape/Backspace**: Back/Exit

## 🚀 Development

```bash
# Install dependencies
npm install

# Start development server
npm run dev

# Build for production
npm run build

# Preview production build
npm run preview
```

## 🎨 Design System

### Colors
- **Background**: Deep black (#0a0a0a) - Tournament venue atmosphere
- **Gold**: (#ffd700) - Success, scores, achievements
- **Cyan**: (#00ffff) - Active inputs, special highlights
- **Red**: (#ff073a) - Failures, errors, damage
- **Orange**: (#ff6b35) - Combo counters
- **Purple**: (#8a2be2) - Perfect inputs

### Typography
- **Primary**: JetBrains Mono (monospace for precision)
- **Display**: Inter (clean, modern headers)

## 🏆 Scoring System

- **Correct Input**: 50 points base
- **Pattern Completion**: 100 point bonus
- **Combo Multiplier**: +10% per consecutive correct input
- **Failure Timeout**: 2-second penalty period

## 📱 Progressive Web App

The app includes PWA features:
- Offline functionality
- Install to home screen
- Fullscreen gaming experience
- Background sync for scores

## 🎯 Roadmap

- [ ] Online leaderboards
- [ ] Custom pattern creation
- [ ] Sound effects and music
- [ ] Advanced statistics
- [ ] Tournament bracket system
- [ ] Replay system
- [ ] Social sharing

## 🤝 Contributing

Based on the original C/SDL3 kbd-trainer implementation. This web version maintains the same core game logic while providing a modern, accessible interface.

## 📄 License

MIT License - See original project for details.
