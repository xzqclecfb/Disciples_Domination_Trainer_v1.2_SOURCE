# 🎮 Disciples: Domination - Trainer

[![Platform](https://img.shields.io/badge/Platform-Windows%2010%2F11-blue)](https://www.microsoft.com/windows)
[![Architecture](https://img.shields.io/badge/Architecture-x64-green)](https://github.com)
[![License](https://img.shields.io/badge/License-Educational-orange)](LICENSE)
[![Language](https://img.shields.io/badge/Language-C%2B%2B17-red)](https://isocpp.org/)

**Professional game trainer for Disciples: Domination (2026)**

Modern, standalone trainer with clean UI and advanced memory manipulation features.

![Screenshot](screenshot.png)

---

## ✨ Features

### 📦 Resources (8 types)
- **Wood** - Building resource
- **Gold** - Currency
- **Iron** - Crafting material
- **Skill Points** - Character development
- **Arcane Flux** - Magical energy
- **Primal Essence** - Primary essence
- **Divine Essence** - Holy essence
- **Unholy Essence** - Dark essence

**Each resource supports:**
- ✏️ Custom value input
- 🔒 Freeze/Lock functionality
- ⚡ Instant application

### ⚔️ Combat & Experience (5 features)
- **God Mode** - Complete invincibility
- **Infinite Squad Health** - Unlimited HP for all units
- **Unlimited Mana** - Infinite energy
- **One-Hit Kill** - Instant enemy elimination
- **Max Experience** - Accelerated leveling

---

## 📥 Download

**[Download Latest Release v1.1](../../releases/latest)**

- **Size:** ~130 KB
- **Format:** Standalone EXE
- **Dependencies:** None required

---

## 🚀 Quick Start

### Requirements
- Windows 10/11 (x64)
- Disciples: Domination game
- Administrator privileges

### Usage
1. Launch **Disciples: Domination**
2. Run trainer **as Administrator**
3. Click **"CONNECT TO GAME"**
4. Modify desired values
5. Click **"SET"** to apply

### Video Tutorial
[![Watch Tutorial](https://img.youtube.com/vi/TUTORIAL_ID/0.jpg)](https://www.youtube.com/watch?v=TUTORIAL_ID)

---

## 🛠️ Building from Source

### Prerequisites
- **Visual Studio 2022** (Community or higher)
- **Windows SDK 10.0** or newer
- **Platform Toolset:** v143

### Build Steps

```bash
# Clone repository
git clone https://github.com/YOUR_USERNAME/disciples-domination-trainer.git
cd disciples-domination-trainer

# Open project
start "Disciples Domination trainer.vcxproj"

# In Visual Studio:
# 1. Select Release | x64
# 2. Build → Build Solution (Ctrl+Shift+B)
# 3. Output: x64\Release\Disciples Domination trainer.exe
```

### Build Configuration
- **Runtime:** Static (/MT)
- **Optimization:** Maximum Speed (/O2)
- **Character Set:** Unicode
- **Platform:** x64 only

---

## 📁 Project Structure

```
Disciples-Domination-Trainer/
├── Disciples Domination trainer/
│   ├── main.cpp                    # Main application & UI
│   ├── MemoryManager.h             # Memory scanning & manipulation
│   ├── resource.h                  # Resource definitions
│   ├── Disciples Domination trainer.vcxproj
│   └── x64/Release/
│       └── Disciples Domination trainer.exe
├── docs/
│   ├── HOW_TO_USE.txt             # User guide
│   └── BUILD_INSTRUCTIONS.md      # Developer guide
└── README.md                       # This file
```

---

## 🎨 Technical Details

### Architecture
- **UI Framework:** Win32 API (native)
- **Rendering:** GDI with gradient effects
- **Memory Access:** ReadProcessMemory/WriteProcessMemory
- **Process Detection:** ToolHelp32 API

### Features Implementation
- **Automatic Process Detection** - Tries 4 process name variants
- **Memory Freezing** - Timer-based continuous write (100ms interval)
- **Dark Modern UI** - Custom-painted controls with RGB(18,18,20) theme
- **Standalone Build** - No external dependencies required

### Tested On
- ✅ Windows 10 21H2, 22H2
- ✅ Windows 11 21H2, 22H2, 23H2
- ✅ Fresh Windows install (no VC++ redist)

---

## ⚠️ Disclaimer

**FOR EDUCATIONAL PURPOSES ONLY**

This trainer is created for:
- Learning Win32 API programming
- Understanding process memory manipulation
- Demonstrating modern C++ techniques

**Important:**
- ✅ Use only in **single-player** mode
- ❌ **DO NOT** use in multiplayer/online
- 💾 Save your game before using
- 🔄 Addresses may change after game updates

**We are not responsible for:**
- Corrupted save files
- Game crashes
- Account bans (if used online)
- Any other damages

---

## 🐛 Known Issues

- **Game updates** - Offsets need updating via Cheat Engine
- **Antivirus false positives** - Add to exclusions
- **Some addresses placeholder** - Requires manual CE scanning

### Reporting Issues
Please include:
- Game version
- Trainer version
- Windows version
- Error message/screenshot

---

## 🤝 Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open Pull Request

### Development Guidelines
- Follow existing code style
- Comment complex logic
- Test on clean Windows install
- Update documentation

---

## 📝 Changelog

### v1.1 (February 2026)
- ✨ Modern dark UI with perfect alignment
- 🎯 8 resource types support
- ⚔️ 5 combat features
- 🔒 Freeze/Lock functionality
- 🚀 Standalone build (no dependencies)
- 📏 Improved UI layout with pixel-perfect positioning

### v1.0 (Initial Release)
- Basic functionality
- Simple UI

---

## 📧 Contact

- **GitHub Issues:** [Report Bug](../../issues)
- **Discussions:** [Community Forum](../../discussions)
- **Email:** your.email@example.com

---

## 📜 License

This project is licensed under the **Educational Use License** - see [LICENSE](LICENSE) file.

**TL;DR:**
- ✅ Learn from code
- ✅ Fork and modify
- ❌ Commercial use prohibited
- ❌ No warranty provided

---

## 🙏 Acknowledgments

- **Cheat Engine Community** - For memory scanning tools
- **FLiNG Trainers** - UI design inspiration
- **Win32 API Documentation** - Microsoft MSDN

---

## ⭐ Support the Project

If you find this useful:
- ⭐ Star this repository
- 🐛 Report bugs
- 💡 Suggest features
- 🔀 Contribute code

---

<div align="center">

**Made with ❤️ for the gaming community**

[Download](../../releases) • [Documentation](docs/) • [Report Bug](../../issues)

</div>
