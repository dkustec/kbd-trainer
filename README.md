# KBD Trainer 🥊

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/dkustec/kbd-trainer/actions)
[![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)](https://github.com/dkustec/kbd-trainer/releases)
[![Language](https://img.shields.io/badge/language-C-orange.svg)](https://github.com/dkustec/kbd-trainer)

> A precision training tool for mastering Korean Backdash (KBD) and Wavedash (WD) inputs in Tekken fighting games.

## 🎯 Features

- **🎮 Multiple Training Modes**
  - P1/P2 Korean Backdash (KBD)
  - P1/P2 Wavedash (WD)
- **⚡ Real-time Input Validation** with visual feedback
- **🏆 Score Tracking** and high score persistence
- **🎛️ Controller Support** (Xbox/PlayStation compatible)
- **⌨️ Keyboard Fallback** (WASD controls)
- **📊 Miss Analysis** with 2-second pause for reflection
- **🎯 Overlay Mode** - Inject into Tekken for in-game training

## 🚀 Quick Start

### Prerequisites

- **CMake** 3.10 or higher
- **Git** (for submodules)
- **Visual Studio 2019+** or compatible C compiler
- **Administrator privileges** (for overlay injection)

### Installation

```bash
# Clone the repository
git clone https://github.com/dkustec/kbd-trainer.git
cd kbd-trainer

# Initialize SDL dependencies
git submodule update --init --recursive

# Build standalone version
mkdir build
cd build
cmake ..
cmake --build .

# Build overlay version (optional)
cmake -DBUILD_OVERLAY=ON ..
cmake --build .
```

### Running

#### Standalone Mode
```bash
# From project root
./bin/KBDTrainer.exe
```

#### Overlay Mode
```bash
# 1. Start Tekken 7/8 or other supported fighting game
# 2. Run as Administrator:
./bin/KBDTrainerInjector.exe

# 3. In-game, press F1 to toggle overlay
```

## 🎮 Controls

### Standalone Mode
| Action | Controller | Keyboard |
|--------|------------|----------|
| Navigate Menu | D-Pad Left/Right | A/D |
| Confirm | A Button | Enter |
| Back/Exit | B/Start | Escape |
| Training Input | D-Pad | WASD |

### Overlay Mode
| Action | Key |
|--------|-----|
| Toggle Overlay | F1 |
| Hide Overlay | Click "Hide" button |

**Overlay automatically detects game inputs - no manual input required!**

## 🎯 Training Modes

### Korean Backdash (KBD)

Perfect for mastering the essential defensive technique in Tekken.

- **P1 Side**: `← N ← ↙`
- **P2 Side**: `→ N → ↘`

### Wavedash (WD)

Master the aggressive movement option for rushdown play.

- **P1 Side**: `→ N ↓ ↘ → N`
- **P2 Side**: `← N ↓ ↙ ← N`

**Legend:** N = Neutral, ← = Back, → = Forward, ↓ = Down, ↙ = Down-Back, ↘ = Down-Forward

## 🏗️ Architecture

Built with modern C and SDL3 for optimal performance:

- **Core Engine**: SDL3 (Graphics, Input, Audio)
- **Image Processing**: SDL3_image (Asset loading)
- **Typography**: SDL3_ttf (Score rendering)
- **Input System**: XInput + SDL gamepad support
- **Build System**: CMake with automated dependency management

## 🛠️ Development

### Project Structure

```text
kbd-trainer/
├── src/           # Core game logic
├── include/       # Header files
├── assets/        # Game assets (sprites, fonts)
├── bin/           # Built executable + DLLs
├── build/         # CMake build directory
└── SDL*/          # Git submodules (auto-managed)
```

### Building from Source

The project uses git submodules to automatically manage SDL dependencies, eliminating the need for manual library installation.

```bash
# Development build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## 🎨 Features in Detail

### Precision Training System

- **Frame-perfect input detection** for competitive accuracy
- **Visual feedback** for correct/incorrect inputs
- **Progressive difficulty** as you build muscle memory

### Smart Pause System

- **2-second reflection period** after mistakes
- **Prevents panic mashing** and promotes deliberate practice
- **Maintains focus** on proper technique

### Score System

- **+50 points** per correct input
- **Persistent high scores** across sessions
- **Progress tracking** for each training mode

## 🚧 Roadmap

- [ ] **Training Analytics** - Input timing analysis
- [ ] **Custom Patterns** - User-defined input sequences
- [ ] **Replay System** - Record and review sessions
- [ ] **Online Leaderboards** - Community competition
- [ ] **Mobile Support** - Cross-platform training
- [x] **Overlay Mode** - In-game training assistance via DLL injection

## 🎯 Overlay Features

The overlay system transforms KBD Trainer into an injectable training assistant:

### 🔧 **Technical Implementation**
- **DLL Injection** - Injects into running fighting games
- **DirectX 11 Hooking** - Intercepts graphics calls using MinHook
- **ImGui Interface** - Modern, responsive overlay UI
- **Real-time Monitoring** - Automatic input pattern detection
- **Non-intrusive** - Doesn't interfere with game performance

### 🎮 **Supported Games**
- Tekken 8 (`TekkenGame-Win64-Shipping.exe`)
- Tekken 7 (`Tekken7-Win64-Shipping.exe`) 
- Street Fighter 6 (`SF6.exe`)
- Street Fighter V (`StreetFighterV.exe`)

### 🚀 **Quick Setup**
```bash
# Build overlay version
./build-overlay.bat

# Usage
1. Start your fighting game
2. Run bin/KBDTrainerInjector.exe as Administrator  
3. Press F1 in-game to toggle overlay
```

## 📦 Packaging & Distribution

### Creating Release Packages

The project supports multiple packaging formats:

```bash
# ZIP package (cross-platform)
cmake --build . --config Release
cpack -G ZIP

# Windows NSIS Installer (requires NSIS)
cpack -G NSIS

# Windows MSI (requires WiX Toolset)
cpack -G WIX
```

### Package Contents
- **Executable**: `KBDTrainer.exe`
- **Libraries**: SDL3 runtime DLLs
- **Assets**: Game sprites and fonts
- **Documentation**: README, installation guide
- **Setup Assistant**: Interactive batch script

### Distribution Platforms
- **GitHub Releases**: Automated ZIP packages
- **Direct Download**: Portable executables
- **Future**: Steam, Microsoft Store, Package managers

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Original concept by [srstafford1996](https://github.com/srstafford1996/kbd-trainer)
- SDL Development Team for the excellent multimedia framework
- Tekken community for inspiring precision training tools

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/dkustec/kbd-trainer/issues)
- **Discussions**: [GitHub Discussions](https://github.com/dkustec/kbd-trainer/discussions)
- **Discord**: [Fighting Game Trainers](https://discord.gg/fightnggames)

---

## Made with ❤️ for the FGC

*Train hard, play harder!* 🥊
