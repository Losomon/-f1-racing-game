# F1 Racing Simulator – First Attempt (Learning Project)

[![Build](https://img.shields.io/badge/Build-Working-brightgreen)]
[![Language](https://img.shields.io/badge/C++-17-blue)]
[![Library](https://img.shields.io/badge/Raylib-GameDev-orange)]

## My First Attempt at Building an F1 Sim

Hi, I'm a solo indie developer and this project is my **first attempt at building a 3D racing simulator in C++ using raylib**.

I wanted to understand how real game engines work, so instead of using Unity or Unreal, I tried to build my own small engine from scratch.  
This turned out to be much harder than I expected.

I had to deal with many problems like:

- Model loading errors
- Wrong file paths
- Engine crashes
- Rendering bugs
- GLB / glTF issues
- Folder structure problems
- Debugging C++ code for hours

Sometimes the game would not even start because of one wrong path.

Even though it was difficult, this project helped me learn how real-time rendering and game engines work internally.

---

## Current Demo (Work in Progress)

Right now the project can:

- Open a window using raylib
- Load 3D models (.glb)
- Render an F1 car model
- Render a track model
- Move camera in the scene
- Use a simple engine structure

This is still a learning project and not a full game yet.

---

## Project Structure

f1-game/
├── .gitignore
├── .vscode/
│   └── c_cpp_properties.json
├── assets/
│   ├── audio/                          # Game audio files (e.g., engine sounds, music)
│   └── models/
│       ├── car/
│       │   └── 2020-f1-mercedes-benz-w11/
│       │       ├── source/
│       │       │   └── 2020 F1 Mercedes-Benz W11.glb
│       │       └── textures/
│       │           ├── 2020_f1_mercedes_w11_badges.etc_3.png
│       │           ├── 2020_f1_mercedes_w11_cab.etc_5.png
│       │           ├── 2020_f1_mercedes_w11_ext_44.etc_4.png
│       │           ├── 2020_f1_mercedes_w11_lights.etc_6.png
│       │           ├── 2020_f1_mercedes_w11_misc.etc_0.png
│       │           ├── 2020_f1_mercedes_w11_wheel.etc_1.png
│       │           ├── car_chassis.etc_8.png
│       │           ├── car_tyre_slick_pirelli_f1_2019.etc_2.png
│       │           └── car_windows.etc_7.png
│       ├── city/
│       │   ├── source/
│       │   │   └── Untitled.glb
│       │   └── textures/               # 100+ building/environment textures (Image_*.png, etc.)
│       └── track/
│           └── source/
│               └── trakmonaco.glb      # Monaco track model
├── out/                                # Build artifacts
├── src/                                # C++ source (physics, rendering, game logic)
│   ├── main.cpp
│   ├── audio/ (AudioManager.hpp/cpp, miniaudio.h)
│   ├── core/ (ECS.hpp, Engine.hpp/cpp, EventBus.hpp)
│   ├── game/ (Car.hpp/cpp, DRS.hpp/cpp, RaceManager.hpp/cpp, Track.hpp/cpp)
│   ├── physics/ (Aerodynamics.hpp, BrakingSystem.hpp, EngineDrivetrain.hpp, F1Constants.hpp, Suspension.hpp/cpp, Tire*.hpp, Vehicle*.hpp/cpp, WeightTransfer.hpp, VehicleConstants.hpp)
│   ├── render/ (Camera.hpp, Renderer.hpp/cpp, ShaderManager.hpp/cpp)
│   └── steam/ (SteamManager.hpp/cpp)
├── tools/
│   ├── generate_ibl.bat
│   └── ibl_baker.py
├── CMakeLists.txt
├── Build scripts: compile.bat, compile2.bat, compile_temp.bat, run_build.bat, build.ps1
├── Docs: README.md, TODO.md, PHASE3-FIX-TODO.md, PHASE

Important:

Assets must be inside runtime folder:


out/assets/models/


Otherwise the game will not load models.

---

## Build & Run

Requirements:

- C++17
- raylib
- g++

Run:


compile.bat


or


g++ main.cpp -lraylib -lopengl32 -lgdi32 -lwinmm


Then run:


f1_racing.exe


---

## Challenges I Faced

This was my first time making something like this and I struggled with:

- Failed to open GLB files
- Working directory problems
- Installing extensions
- Fixing engine code
- Understanding raylib model loading
- Debugging for many hours

But solving these problems helped me understand game development much better.

---

## Future Plans

- Add car movement
- Add basic physics
- Add collisions
- Improve camera
- Add UI
- Build a small racing game

---

##  Contribute
PRs welcome! Issues for bugs/track requests. Star if you lap Monaco under 1:10!

**Built with love for F1 fans. First try, but hooked for life.**

---

*© 2026 Solo Dev. MIT License.
