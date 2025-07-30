# KBD Trainer Installation Guide

## 📦 Package Options

### ZIP Package (Current)
- **File**: `KBD Trainer-1.0.0-win64.zip`
- **Size**: ~2.3 MB
- **Requirements**: Windows 10/11 (64-bit)
- **Installation**: Extract and run

### Future Package Options
- **NSIS Installer**: Professional Windows installer with Start Menu shortcuts
- **WiX MSI**: Enterprise-ready Windows installer
- **Portable**: Self-contained executable

## 🚀 Quick Installation (ZIP)

### Method 1: Manual Installation
1. **Download** the `KBD Trainer-1.0.0-win64.zip` file
2. **Extract** to your desired location (e.g., `C:\Games\KBD Trainer\`)
3. **Run** `KBDTrainer.exe` from the extracted folder

### Method 2: PowerShell Installation
```powershell
# Download and extract (replace URL with actual download link)
Invoke-WebRequest -Uri "https://github.com/dkustec/kbd-trainer/releases/download/v1.0.0/KBD-Trainer-1.0.0-win64.zip" -OutFile "KBD-Trainer.zip"
Expand-Archive -Path "KBD-Trainer.zip" -DestinationPath "C:\Games\"
```

## 📂 Package Contents

The package includes:
- `KBDTrainer.exe` - Main application
- `SDL3.dll` - Graphics and input library
- `SDL3_image.dll` - Image loading library  
- `SDL3_ttf.dll` - Font rendering library
- `assets/` - Game sprites and fonts
- `README.md` - Documentation

## 🎮 System Requirements

### Minimum Requirements
- **OS**: Windows 10 (64-bit) or newer
- **Memory**: 256 MB RAM
- **Graphics**: DirectX 11 compatible
- **Input**: Keyboard or Xbox/PlayStation controller
- **Storage**: 50 MB available space

### Recommended Requirements
- **OS**: Windows 11 (64-bit)
- **Memory**: 512 MB RAM
- **Graphics**: Dedicated GPU with DirectX 12
- **Input**: Xbox/PlayStation controller for best experience
- **Storage**: 100 MB available space

## 🔧 Advanced Installation Options

### Installing NSIS for Installer Creation
If you want to build the NSIS installer:

1. **Download NSIS** from: https://nsis.sourceforge.io/Download
2. **Install NSIS** to default location
3. **Add to PATH** (usually automatic)
4. **Build installer**:
   ```bash
   cmake --build . --config Release
   cpack -G NSIS
   ```

### Creating Desktop Shortcut
```batch
@echo off
set "TARGET=%~dp0KBDTrainer.exe"
set "SHORTCUT=%USERPROFILE%\Desktop\KBD Trainer.lnk"
powershell "$s=(New-Object -COM WScript.Shell).CreateShortcut('%SHORTCUT%'); $s.TargetPath='%TARGET%'; $s.Save()"
echo Desktop shortcut created!
pause
```

## 🎯 First Run

1. **Start the application** by double-clicking `KBDTrainer.exe`
2. **Connect a controller** (optional but recommended)
3. **Select training mode**:
   - P1/P2 Korean Backdash (KBD)
   - P1/P2 Wavedash (WD)
4. **Begin training!**

## 🛠️ Troubleshooting

### Application Won't Start
- **Check dependencies**: Ensure all DLLs are in the same folder
- **Update drivers**: Install latest graphics drivers
- **Run as admin**: Right-click and "Run as administrator"

### Controller Not Detected
- **Use USB connection** for most reliable detection
- **Check Windows Game Controllers**: Settings > Gaming > Game Controller settings
- **Try keyboard controls**: WASD for movement

### Performance Issues
- **Close other applications** to free memory
- **Update graphics drivers**
- **Lower system visual effects**

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/dkustec/kbd-trainer/issues)
- **Documentation**: [README.md](README.md)
- **Community**: [GitHub Discussions](https://github.com/dkustec/kbd-trainer/discussions)

---

*Happy training! 🥊*
