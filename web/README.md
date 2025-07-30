# KBD Trainer Web v2.0

Modern web-based Korean Backdash and Wavedash training application with enhanced features and gaming-style UI.

## 🎮 Features

### Enhanced Training Modes
- **Player 1 KBD** - Korean Backdash training for Player 1 side
- **Player 2 KBD** - Korean Backdash training for Player 2 side  
- **Advanced KBD** - Extended Korean Backdash sequences
- **Wavedash** - Wavedash timing training

### Advanced Capabilities
- **Frame-Perfect Timing** - Real-time frame measurement and display
- **Enhanced Scoring** - Advanced scoring system with Perfect/Good/Miss ratings
- **Visual Feedback** - Dynamic UI effects and floating feedback messages
- **Multiple Input Methods** - Keyboard, gamepad, and analog stick support
- **Performance Monitoring** - FPS tracking and input rate analysis

### Modern UI/UX
- **Gaming Aesthetic** - Violet/purple color scheme with glowing effects
- **Responsive Design** - Works on desktop, tablet, and mobile devices
- **Real-time Visualization** - Timing bars and input sequence display
- **Accessibility** - Keyboard shortcuts and screen reader friendly

## 🕹️ Controls

### Keyboard Controls
- **Arrow Keys** - Directional input (↑↓←→)
- **WASD Keys** - Alternative directional input
- **Numpad** - Full 8-way directional input support
- **Number Keys 1-4** - Quick mode selection
- **Space** - Start/Stop training
- **R** - Reset training session
- **ESC** - Stop current training

### Gamepad Support
- **D-Pad** - Digital directional input
- **Left Analog Stick** - 8-way analog directional input
- **Auto-detection** - Automatically detects connected gamepads

## 🎯 Training Modes

### P1/P2 Korean Backdash
- **Sequence**: Back → Down → Down-Back → Back
- **Perfect Window**: 16ms
- **Good Window**: 33ms
- **Scoring**: Perfect (100pts), Good (50pts), Miss (-10pts)

### Advanced KBD
- **Sequence**: Extended KBD combo sequence
- **Perfect Window**: 12ms (tighter timing)
- **Good Window**: 25ms
- **Scoring**: Perfect (150pts), Good (75pts), Miss (-15pts)

### Wavedash
- **Sequence**: Forward → Down → Down-Forward
- **Perfect Window**: 20ms
- **Good Window**: 40ms
- **Scoring**: Perfect (80pts), Good (40pts), Miss (-5pts)

## 📊 Scoring System

### Timing Ratings
- **Perfect** - Input within perfect timing window
- **Good** - Input within acceptable timing window
- **Miss** - Input outside timing window or wrong input

### Statistics Tracking
- **Score** - Total accumulated points
- **Perfect Hits** - Count of perfect-timed inputs
- **Good Hits** - Count of good-timed inputs
- **Missed Hits** - Count of missed or incorrect inputs
- **Accuracy** - Overall accuracy percentage
- **Frame Time** - Average frame timing in milliseconds

## ⚙️ Settings

### Timing Sensitivity
- **Range**: 1-10
- **Default**: 5
- **Effect**: Adjusts timing window strictness

### Target Frame Time
- **Range**: 16-100ms
- **Default**: 33ms (30 FPS target)
- **Effect**: Sets the ideal timing target for inputs

### Audio Volume
- **Range**: 0-100%
- **Default**: 50%
- **Effect**: Controls feedback sound volume

## 🔧 Technical Features

### Performance Monitoring
- **Real-time FPS** - Frame rate tracking
- **Input Rate** - Inputs per second measurement
- **Memory Usage** - JavaScript heap monitoring
- **Frame Timing** - Precise input timing measurement

### Architecture
- **Modular Design** - Separate game engine, input handler, and UI controller
- **Event-Driven** - Custom event system for component communication
- **Performance Optimized** - RequestAnimationFrame-based rendering
- **Error Handling** - Comprehensive error recovery and user feedback

### Browser Compatibility
- **Modern Browsers** - Chrome, Firefox, Safari, Edge
- **ES6+ Features** - Uses modern JavaScript features
- **WebAPIs** - Gamepad API, Performance API, RequestAnimationFrame
- **Responsive** - Mobile and desktop optimized

## 🚀 Getting Started

1. **Open** `index.html` in a modern web browser
2. **Select** a training mode by clicking one of the mode buttons
3. **Configure** settings if desired using the settings panel
4. **Start** training by clicking "Start Training" or pressing Space
5. **Practice** using keyboard or gamepad inputs
6. **Monitor** your performance with real-time feedback

## 🎨 Visual Design

### Color Scheme
- **Primary**: Purple/Violet gradients (#8b5cf6, #a855f7)
- **Accent**: Cyan highlights (#06b6d4)
- **Background**: Dark theme (#0f0f1a, #1a1a2e)
- **Success**: Green (#10b981)
- **Warning**: Orange (#f59e0b)
- **Error**: Red (#ef4444)

### Typography
- **Headings**: Orbitron (futuristic, gaming font)
- **Body**: Rajdhani (clean, modern sans-serif)
- **UI Elements**: High contrast, readable sizing

### Effects
- **Glow Effects** - Neon-style borders and shadows
- **Smooth Animations** - CSS transitions and transforms
- **Visual Feedback** - Color-coded timing feedback
- **Responsive Design** - Fluid layouts and scaling

## 🐛 Debug Features

### Console Helpers
- `window.kbdTrainer()` - Access app instance
- `window.exportKBDStats()` - Export training statistics
- `window.debugKBD` - Debug utility functions

### Debug Commands
```javascript
// Simulate inputs for testing
debugKBD.simulateInput('b');  // Simulate back input
debugKBD.simulateInput('d');  // Simulate down input

// Control training
debugKBD.setMode('KBD');       // Set training mode
debugKBD.start();              // Start training
debugKBD.stop();               // Stop training
debugKBD.reset();              // Reset session

// Get performance stats
debugKBD.getStats();           // Current performance metrics
```

## 📈 Performance Tips

### Optimal Setup
- **Use Chrome/Firefox** for best performance
- **Connect gamepad** before starting training
- **Close unnecessary tabs** to reduce system load
- **Use fullscreen mode** for distraction-free training

### Input Lag Reduction
- **Disable browser extensions** that might interfere
- **Use wired gamepad** for lowest latency
- **Reduce browser zoom** to default 100%
- **Close other applications** using significant CPU/GPU

## 🔄 Future Enhancements

### Planned Features
- **Custom Sequences** - User-defined training patterns
- **Replay System** - Record and playback training sessions
- **Online Leaderboards** - Global and local rankings
- **Training Plans** - Structured practice routines
- **Audio Cues** - Metronome and feedback sounds
- **Export/Import** - Save and share training data

### Technical Improvements
- **WebAssembly** - Core engine optimization
- **Web Workers** - Background processing
- **PWA Features** - Offline support and app installation
- **WebRTC** - Multiplayer training sessions

---

**Version**: 2.0.0  
**Technology**: HTML5, CSS3, JavaScript ES6+  
**License**: MIT  
**Author**: KBD Trainer Development Team

Built with ❤️ for the Tekken community 🥋⚡
