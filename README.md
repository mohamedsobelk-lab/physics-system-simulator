# Physics System Simulator

A C++ physics simulation using the Box2D engine with real-time debug visualization, performance profiling, and interactive controls.

## 📖 Overview

This project demonstrates the integration of a professional physics engine (Box2D) into a C++ application. It features a complete debug drawing system, performance monitoring, and interactive controls to manipulate the simulation in real-time.

## 🛠️ Technical Implementation

- **Language:** C++17
- **Physics Engine:** Box2D (v3.1.0)
- **Graphics:** SFML (Simple and Fast Multimedia Library) v3.0.1
- **Core Concepts:** Physics simulation, debug visualization, performance profiling
- **Build System:** CMake (without vcpkg)

## ✨ Features & Demonstrations

This project provides tangible evidence of the following skills:

| Feature | Implementation | Relevance to Engine Programming |
| :--- | :--- | :--- |
| **Physics System Integration** | Full integration of Box2D physics engine with proper world stepping and body management. | Directly demonstrates experience with **physics** core functionality in game engines. |
| **Debug Visualization** | Custom `PhysicsDebugDraw` class that implements Box2D's `b2Draw` interface for real-time debug rendering. | Shows ability to create **great debugging tools** for visualizing engine systems. |
| **Performance Profiling** | Real-time measurement and display of physics step time, demonstrating performance awareness. | Evidence of focus on **performance-critical** systems and optimization. |
| **Interactive Tools** | Live creation/destruction of physics bodies, reset functionality, and parameter control. | Demonstrates creation of **developer tools** that improve iteration times. |
| **Memory Management** | Proper cleanup of Box2D bodies and world management. | Shows understanding of **memory characteristics** in engine programming. |

## 🎮 Controls

- **SPACE**: Add a new physics object to the simulation
- **R**: Reset the simulation with default objects
- **ESC**: Exit the application

## 📊 Performance Metrics

The simulation displays real-time performance data:
- Number of active physics objects
- Current physics step time (ms)
- Average physics step time (ms)
- Framerate (capped at 60 FPS)

## 🚀 Building and Running

### Prerequisites
- **C++17 compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake 3.20+**
- **Box2D 3.1.0** (built from source or precompiled)
- **SFML 3.0.1** (built from source or precompiled)

### Installation Steps

1. **Install Dependencies:**
   - Download and build Box2D 3.1.0 from: https://github.com/erincatto/box2d
   - Download and build SFML 3.0.1 from: https://github.com/SFML/SFML

2. **Configure the project:**
```bash
mkdir build
cd build
cmake .. -DBOX2D_ROOT=/path/to/box2d -DSFML_ROOT=/path/to/sfml
```

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Copyright (c) 2024 Mohamed Sobhy Elkharashy
