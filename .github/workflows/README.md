# GitHub Actions Workflows

This directory contains automated workflows for the KBD Trainer project.

## 🔄 Workflows

### `build-and-release.yml`
**Main build and release workflow**

**Triggers:**
- Push to `main` branch - Creates build artifacts
- Push tags (v*) - Creates GitHub release with packages

**What it does:**
1. Sets up Windows build environment (MSVC, NSIS, WiX)
2. Builds the project with CMake
3. Creates all three package types:
   - ZIP archive (portable)
   - NSIS installer (.exe)
   - WiX MSI package
4. Uploads build artifacts
5. Creates GitHub release (on tag push)

**Artifacts:** Available for 30 days in Actions tab

### `test-build.yml`
**Pull request testing**

**Triggers:**
- Pull requests to `main` branch

**What it does:**
1. Quick build test to ensure code compiles
2. Verifies executable is created correctly
3. No packaging (faster feedback)

## 🚀 How to Use

### For Development
1. Push to `main` branch
2. Check Actions tab for build status
3. Download artifacts to test packages

### For Releases
1. Create and push a version tag:
   ```bash
   git tag v1.0.0
   git push --tags
   ```
2. GitHub will automatically:
   - Build all packages
   - Create a release
   - Upload installers
   - Generate release notes

## 📦 Package Outputs

| Package Type | File Pattern | Size | Use Case |
|-------------|--------------|------|----------|
| ZIP Archive | `KBD Trainer-*-win64.zip` | ~2.2 MB | Portable/Testing |
| NSIS Installer | `KBD Trainer-*-win64.exe` | ~1.8 MB | End Users |
| MSI Package | `KBD Trainer-*-win64.msi` | ~2.5 MB | Enterprise |

## 🛠️ Dependencies

The workflows automatically install:
- **NSIS v3.11** - For .exe installer creation
- **WiX Toolset v3.14** - For .msi package creation
- **MSVC 2022** - For compilation

## 📋 Requirements

- Repository must have submodules properly configured
- SDL dependencies included as submodules
- CMake build system configured
- No additional secrets required (uses GITHUB_TOKEN)
