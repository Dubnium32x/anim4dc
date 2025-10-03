# Anim4DC - Dreamcast Raylib Animation Plugin

## Project Overview
This is a standalone animation plugin for KOS/Dreamcast development with raylib 5.5. The plugin provides optimized vertex animation systems specifically designed for Dreamcast hardware limitations.

## Core Features
- **Vertex Animation Baking**: Converts skeletal animations to vertex keyframes for optimal Dreamcast performance
- **Multi-format Support**: FBX, GLB, GLTF, IQM, OBJ model loading with intelligent fallback chains
- **LOD System**: Distance-based Level-of-Detail rendering and culling optimizations
- **Memory Optimization**: Hardware-aware memory management for 16MB Dreamcast RAM constraints
- **Performance Tuning**: Optimized for 25 FPS @ 25 models or 60 FPS @ 3 models on real hardware

## Architecture
- **Platform**: Sega Dreamcast with KallistiOS
- **Renderer**: GLdc (OpenGL ES-like API)
- **Build System**: KOS Makefiles with romdisk integration
- **Testing**: Flycast emulator and real hardware via USB-UART debugging

## Key Components
1. **VertexKeyframe System**: 20 keyframes maximum, optimized interpolation
2. **BakeVertexAnimations()**: Main conversion function from skeletal to vertex
3. **LOD Constants**: Distance-based culling (80²/120²/160²/200² units)
4. **Fox Model Demo**: Survey/Walk/Run animations showcasing the system

## Development Context
Extracted from successful DMS_vs_anim4dc benchmark that achieved smooth Fox animations on Dreamcast hardware. This plugin makes the vertex animation system reusable for community developers.

## Build Requirements
- KallistiOS development environment
- raylib 5.5 with GLdc backend
- SH4 cross-compilation toolchain
- CDI creation tools for hardware testing

## Performance Targets
- Real Hardware: 25 FPS with 12 models (memory optimized)
- Flycast Emulator: 25 FPS with 25 models, 60 FPS with 3 models
- Resolution: 320x240 for optimal performance
- Memory: Under 16MB total usage including textures

## Usage Pattern
This plugin is designed for developers who need efficient 3D animation on Dreamcast while maintaining good performance. Focus on vertex animations over skeletal for hardware compatibility.