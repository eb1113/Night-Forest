# Night Forest

*A graphics-heavy engine and interactive world set in a walkable night-time forest.*

Night Forest is an in-progress graphics engine and exploratory environment built as a capstone-style project. The goal is to experiment with real-time rendering techniques, world interaction, and atmospheric visuals while building core engine systems from the ground up.

> ⚠️ **Status:** Actively under development — many systems are experimental and subject to change.

test
---

## Features (Current & Planned)

### Rendering

* Real-time 3D rendering
* Custom shader pipeline (GLSL)
* Dynamic lighting (moonlight / point lights)
* Atmospheric effects (fog, darkness, tone mapping)
* Camera system (free-look / first-person style)

### World & Interaction

* Walkable forest environment
* Basic player movement
* Scene graph / object management *(in progress)*
* Terrain and object placement *(in progress)*

### Engine Architecture

* Modular C++ engine design
* Windowing and input handling
* Math utilities (vectors, matrices, transforms)
* Asset loading pipeline *(in progress)*

### Planned / Experimental

* Shadow mapping
* Skybox / procedural sky
* Post-processing effects
* Physics or collision detection
* Sound / ambient audio
* Entity Component System (ECS)

---

## Tech Stack

* **Language:** C++
* **Graphics API:** OpenGL
* **Shaders:** GLSL
* **Windowing / Input:** GLFW
* **Math:** GLM
* **Build System:** CMake
* **Platform:** Windows / Linux


## Getting Started

### Prerequisites

* C++17 or newer
* OpenGL 3.3+
* CMake
* GLFW
* GLM

### Build Instructions

```bash
git clone https://github.com/your-username/night-forest.git
cd night-forest
mkdir build && cd build
cmake ..
make
```

### Run

```bash
./NightForest
```

---

## 🎮 Controls (Temporary)

| Action        | Key   |
| ------------- | ----- |
| Move Forward  | W     |
| Move Backward | S     |
| Strafe Left   | A     |
| Strafe Right  | D     |
| Look Around   | Mouse |

*(Bindings may change.)*

---

## Screenshots / Videos

> Coming soon — visuals will be added as the environment and lighting mature.

---

## Design Goals

* Build a low-level understanding of real-time graphics
* Experiment with atmosphere, lighting, and mood
* Create a cohesive world rather than isolated demos
* Keep the engine readable and extensible

---

## Known Issues

* Performance not yet optimized
* Limited error handling
* Asset pipeline incomplete
* Rendering artifacts under certain camera angles

---

## Roadmap

* [ ] Refactor renderer architecture
* [ ] Add shadow mapping
* [ ] Improve terrain generation
* [ ] Introduce ECS
* [ ] Add sound and ambience
* [ ] Polish camera and movement

---


## Author

**Ellie**
Computer Science (Graphics / Systems)

---

*Night Forest is a work in progress — expect rough edges, unfinished systems, and lots of experimentation.* 🌙🌲
