# Semantic Versioning Guide for KBD Trainer

This project follows [Semantic Versioning](https://semver.org/) principles for releases.

## 📋 Version Format

**Format:** `v{MAJOR}.{MINOR}.{PATCH}[-{PRE-RELEASE}][+{BUILD}]`

### Examples:
- `v1.0.0` - Initial stable release
- `v1.1.0` - Minor update with new features
- `v1.0.1` - Patch release with bug fixes
- `v2.0.0` - Major release with breaking changes
- `v1.0.0-alpha` - Pre-release alpha version
- `v1.0.0-beta.1` - Pre-release beta version
- `v1.0.0-rc.2` - Release candidate

## 🎯 When to Increment

### MAJOR version (v1.0.0 → v2.0.0)
- Breaking changes to user interface
- Incompatible changes to save files or settings
- Major architectural changes
- Removal of features

### MINOR version (v1.0.0 → v1.1.0)
- New features that are backward compatible
- New training modes or exercises
- New configuration options
- Performance improvements
- New supported input devices

### PATCH version (v1.0.0 → v1.0.1)
- Bug fixes
- Security patches
- Minor performance optimizations
- Documentation updates
- Typo corrections

### PRE-RELEASE versions
- `alpha` - Early development, may be unstable
- `beta` - Feature complete, testing phase
- `rc` (release candidate) - Final testing before release

## 🚀 How to Create a Release

### 1. **Prepare for Release**
```bash
# Make sure you're on main branch with latest changes
git checkout main
git pull origin main

# Ensure all tests pass and build works
cd build
cmake --build . --config Release
cpack
```

### 2. **Create and Push Tag**
```bash
# For a new feature release
git tag v1.1.0

# For a bug fix release
git tag v1.0.1

# For a pre-release
git tag v1.0.0-beta.1

# Push the tag to trigger automated release
git push --tags
```

### 3. **Automated Process**
Once you push a tag, GitHub Actions will automatically:
- ✅ Validate the semantic version format
- 🔨 Build all packages (ZIP, NSIS, MSI)
- 📋 Generate changelog from commit messages
- 🎉 Create GitHub release with professional description
- 📦 Attach all installer packages

## 🔄 Development Workflow

### Regular Development
```bash
# Push changes to main
git add .
git commit -m "feat: add new training mode"
git push

# Result: Builds development version like v1.0.0-dev.42+a1b2c3d
```

### Release Process
```bash
# Create release tag
git tag v1.1.0
git push --tags

# Result: Creates official release v1.1.0 with packages
```

## 📖 Commit Message Format

For better changelog generation, use conventional commits:

```bash
# Features
git commit -m "feat: add Korean backdash combo trainer"
git commit -m "feat(ui): add dark mode toggle"

# Bug fixes
git commit -m "fix: resolve input lag on Windows 11"
git commit -m "fix(audio): prevent sound crackling"

# Documentation
git commit -m "docs: update installation guide"

# Build/CI
git commit -m "build: update SDL to version 3.2.20"
git commit -m "ci: add automated packaging workflow"
```

## 🏷️ Tag Examples

```bash
# Production releases
git tag v1.0.0    # First stable release
git tag v1.1.0    # New features added
git tag v1.1.1    # Bug fixes
git tag v2.0.0    # Major version with breaking changes

# Pre-releases
git tag v1.0.0-alpha     # Early alpha
git tag v1.0.0-alpha.2   # Second alpha
git tag v1.0.0-beta      # Beta version
git tag v1.0.0-beta.3    # Third beta
git tag v1.0.0-rc.1      # Release candidate
```

## 🔍 Version Detection

The GitHub Actions workflow automatically:
- Detects if the current commit has a version tag
- Generates development versions for untagged commits
- Validates semantic versioning format
- Creates appropriate release types (stable vs pre-release)

### Development Versions
Format: `v{LAST_TAG}-dev.{COMMIT_COUNT}+{SHORT_SHA}`

Example: `v1.0.0-dev.15+a1b2c3d`
- Last tag: `v1.0.0`
- Commits since tag: `15`
- Current commit: `a1b2c3d`

## 📊 Release Types

| Version Pattern | Release Type | Example |
|----------------|--------------|---------|
| `v1.2.3` | Stable | `v1.0.0` |
| `v1.2.3-alpha*` | Pre-release | `v1.0.0-alpha` |
| `v1.2.3-beta*` | Pre-release | `v1.0.0-beta.1` |
| `v1.2.3-rc*` | Pre-release | `v1.0.0-rc.2` |
| `v*-dev.*` | Development | `v1.0.0-dev.5+abc123` |

## 🎉 Quick Start

**To create your first release:**

```bash
# Tag the current commit
git tag v1.0.0

# Push the tag
git push --tags

# Watch GitHub Actions create the release automatically!
```

The automated system will handle everything else, creating professional release notes and attaching all installer packages! 🚀
