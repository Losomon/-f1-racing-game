# 🏎️ F1 Racing Simulator — Monaco

[![Build](https://img.shields.io/badge/Build-Working-brightgreen)]()
[![Language](https://img.shields.io/badge/C++-17-blue)]()
[![Library](https://img.shields.io/badge/Raylib-4.5-orange)]()
[![License](https://img.shields.io/badge/License-MIT-yellow)]()

> A from-scratch 3D F1 racing simulator built in C++ with raylib — featuring the Monaco circuit, a Mercedes W11, real-time rendering, and custom physics.

---

## 📸 Screenshots

<!-- INSTRUCTIONS: Replace the paths below with your actual screenshot files.
     Put your images inside a folder called `docs/screenshots/` in your repo root.
     Example: docs/screenshots/gameplay.png -->

### Main view
<img width="1031" height="477" alt="Image" src="https://github.com/user-attachments/assets/4d5fe95a-8539-4608-84ff-68d828c48223" />

### Car render

<img width="567" height="353" alt="Image" src="https://github.com/user-attachments/assets/507b1048-c109-4677-beaa-d8ee877b5f1e" />

### Monaco track
<img width="642" height="266" alt="Image" src="https://github.com/user-attachments/assets/77f0cad0-eb21-46f3-96c0-30144d5c773e" />

> **Tip:** To add a screenshot, press `PrintScreen` while the game runs,
> save it as a `.png`, place it in `docs/screenshots/`, and commit it to GitHub.

---

## 🎮 Demo (GIF)

<!-- Replace with your actual GIF — record with ScreenToGif (free tool) -->
![Gameplay demo](docs/screenshots/demo.gif)

---

## 🚀 Features

- 3D rendering with [raylib](https://www.raylib.com/)
- Mercedes-Benz W11 F1 car model (`.glb`)
- Monaco circuit track model
- Free-move camera
- Custom ECS (Entity Component System)
- Physics stubs: suspension, aerodynamics, tyre model, braking
- Audio manager (miniaudio)
- DRS system skeleton
- Race manager

---

## 🗂️ Project Structure

```
f1-game/
├── assets/
│   ├── audio/                        # Engine sounds, music
│   └── models/
│       ├── car/
│       │   └── 2020-f1-mercedes-benz-w11/
│       │       ├── source/           # W11.glb
│       │       └── textures/         # PBR texture maps
│       ├── city/                     # Environment / buildings
│       └── track/
│           └── source/trakmonaco.glb # Monaco circuit
├── src/
│   ├── main.cpp
│   ├── core/     # ECS, Engine, EventBus
│   ├── game/     # Car, DRS, RaceManager, Track
│   ├── physics/  # Suspension, Tyres, Aero, Braking
│   ├── render/   # Camera, Renderer, ShaderManager
│   └── audio/    # AudioManager
├── out/          # Build output — assets must be copied here
├── CMakeLists.txt
└── compile.bat
```

> ⚠️ **Important:** Assets must live inside `out/assets/models/` at runtime or models won't load.

---

## 🛠️ Build & Run

### Requirements

| Tool | Version |
|------|---------|
| C++ | 17 or higher |
| raylib | 4.5+ |
| g++ / MinGW | latest |

### Quick build (Windows)

```bat
compile.bat
```

### Manual build

```bash
g++ src/main.cpp -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -o out/f1_racing.exe
```

### Run

```bash
cd out
f1_racing.exe
```

---

## 🗺️ Roadmap

- [x] Window creation
- [x] GLB model loading (car + track)
- [x] Free camera
- [x] ECS architecture
- [x] Physics stubs (suspension, aero, tyres)
- [ ] Car movement & input
- [ ] Collision detection
- [ ] Lap timing system
- [ ] AI opponents
- [ ] HUD (speed, gear, delta)
- [ ] Full race loop

---

## 🧠 What I Learned

Building this from scratch (instead of using Unity/Unreal) taught me:

- How real-time rendering pipelines work internally
- How game engines handle model loading and the working directory
- How ECS architecture separates concerns in a game
- How to debug C++ crashes, path issues, and OpenGL errors
- How physics systems (suspension, tyres, weight transfer) are structured

---

## 🤝 Contributing

PRs welcome! Open an issue for bugs or track requests.

⭐ Star this repo if you lap Monaco under **1:10**!

---

*© 2026 Solo Dev · MIT License · Built with love for F1 fans*
