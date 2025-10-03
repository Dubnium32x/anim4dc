# Anim4DC - Dreamcast Raylib Animation Plugin

🦊 **High-performance vertex animation system optimized for Sega Dreamcast hardware**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Dreamcast-orange.svg)](https://en.wikipedia.org/wiki/Dreamcast)
[![Raylib](https://img.shields.io/badge/Raylib-5.5+-green.svg)](https://raylib.com)

## 🎯 Overview

Anim4DC is a standalone animation plugin that converts skeletal animations to optimized vertex keyframes for smooth 3D animation on Dreamcast hardware. Born from successful benchmarking that achieved **25 FPS with 25 animated models** on Flycast emulator and **optimized performance on real hardware**.

### ✨ Key Features

- **🔧 Vertex Animation Baking**: Converts skeletal animations to vertex keyframes for optimal Dreamcast performance
- **📁 Multi-format Support**: FBX, GLB, GLTF, IQM, OBJ model loading with intelligent fallback chains  
- **🎚️ LOD System**: Distance-based Level-of-Detail rendering and culling optimizations
- **💾 Memory Optimization**: Hardware-aware memory management for 16MB Dreamcast RAM constraints
- **⚡ Performance Tuning**: Optimized for 25 FPS @ 25 models or 60 FPS @ 3 models on real hardware

### 🎮 Performance Targets

| Platform | Models | Target FPS | Resolution | Notes |
|----------|--------|------------|------------|-------|
| **Flycast Emulator** | 25 | 25 FPS | 640x480 | Full demo performance |
| **Flycast Emulator** | 3 | 60 FPS | 640x480 | Smooth gameplay |
| **Real Hardware** | 12 | 25 FPS | 640x480 | Memory optimized |

## 🛠️ Architecture

- **Platform**: Sega Dreamcast with KallistiOS and raylib
- **Renderer**: GLdc (OpenGL ES-like API)  
- **Build System**: KOS Makefiles with romdisk integration
- **Testing**: Flycast emulator and real hardware via USB-UART debugging

## 🚀 Quick Start

### Prerequisites

- KallistiOS development environment
- raylib 5.5 with GLdc backend
- SH4 cross-compilation toolchain
- CDI creation tools for hardware testing (e.g. mkisofs, cdrdao, mkdcdisc (preferred!), or discjuggler)

### Basic Usage

```c
#define ANIM4DC_IMPLEMENTATION
#include "anim4dc.h"

int main(void) {
    // Initialize the animation system
    Anim4dcInit();
    
    // Load model with fallback support
    Model foxModel = Anim4dcLoadModel("/rd/Fox");
    ModelAnimation *animations = LoadModelAnimations("/rd/Fox.gltf", &animCount);
    
    // Bake skeletal animations to vertex keyframes
    Anim4dcBakeVertexAnimations(foxModel, animations, animCount);
    
    // Main loop
    while (!WindowShouldClose()) {
        // Update animation
        Anim4dcUpdateAnimation(GetFrameTime());
        
        // Get interpolated vertices for rendering
        float *vertices = Anim4dcGetInterpolatedVertices();
        
        // Render your animated model
        BeginDrawing();
        // ... render code ...
        EndDrawing();
    }
    
    // Cleanup
    Anim4dcShutdown();
    return 0;
}
```

### Fox Demo

The included Fox demo showcases all features:

```bash
cd examples/fox_demo
make clean && make
make cdi  # Creates fox_demo.cdi for hardware/emulator
```

**Controls:**
- **A Button**: Cycle animations (Survey → Walk → Run)
- **B Button**: Toggle debug information
- **D-Pad**: Rotate camera view
- **Start**: Pause/unpause animation

##### Fox model comes from this git repo:
https://github.com/KhronosGroup/glTF-Sample-Models/tree/main/2.0/Fox

## 📚 API Reference

### Core Functions

#### System Management
```c
bool Anim4dcInit(void);                    // Initialize the animation system
void Anim4dcShutdown(void);               // Cleanup and shutdown
const char *Anim4dcGetVersion(void);      // Get version string
```

#### Animation Baking
```c
bool Anim4dcCheckModelCompatibility(Model model, ModelAnimation *animations, int count);
bool Anim4dcBakeVertexAnimations(Model model, ModelAnimation *animations, int count);
void Anim4dcUpdateAnimation(float deltaTime);
float *Anim4dcGetInterpolatedVertices(void);
```

#### Animation Control
```c
bool Anim4dcSetAnimation(int animationIndex);
bool Anim4dcSetAnimationByName(const char *animationName);
int Anim4dcGetCurrentAnimation(void);
float Anim4dcGetAnimationTime(void);
void Anim4dcSetAnimationTime(float time);
```

#### Performance Optimization
```c
void Anim4dcUpdateInstanceLOD(Anim4dcModelInstance *instances, int count, Vector3 cameraPos);
void Anim4dcRenderInstances(Model model, Anim4dcModelInstance *instances, int count);
Anim4dcStats Anim4dcGetStats(void);
```

### Data Structures

#### VertexKeyframe
```c
typedef struct Anim4dcVertexKeyframe {
    float *vertices;        // Vertex positions for this keyframe
    int vertexCount;       // Number of vertices  
    float timestamp;       // Time for this keyframe in seconds
} Anim4dcVertexKeyframe;
```

#### VertexAnimation
```c
typedef struct Anim4dcVertexAnimation {
    char name[32];                          // Animation name
    Anim4dcVertexKeyframe keyframes[20];    // Keyframe data (max 20)
    int keyframeCount;                      // Number of keyframes
    float duration;                         // Total animation duration
    bool looping;                          // Should animation loop?
} Anim4dcVertexAnimation;
```

#### ModelInstance (for batch rendering)
```c
typedef struct Anim4dcModelInstance {
    Vector3 position;           // World position
    Vector3 rotation;           // Euler rotation angles
    float scale;               // Uniform scale
    int animationIndex;        // Which animation to play
    float animationTime;       // Current animation time
    Anim4dcLodLevel lodLevel;  // Current LOD level
    bool visible;              // Should be rendered
    float distanceSquared;     // Distance from camera (squared)
} Anim4dcModelInstance;
```

## 🎨 LOD System

The Level-of-Detail system automatically optimizes performance based on distance:

| LOD Level | Distance | Animation Speed | Rendering |
|-----------|----------|-----------------|-----------|
| **NEAR** | < 80 units | 100% (1.0x) | Full detail |
| **MID** | 80-120 units | 50% (0.5x) | Reduced rate |
| **FAR** | 120-160 units | 25% (0.25x) | Minimal |
| **FROZEN** | 160-200 units | 0% (0.0x) | Static |
| **CULLED** | > 200 units | N/A | Not rendered |

## 🔧 Building

### Fox Demo
```bash
cd examples/fox_demo
make clean && make     # Build ELF
make cdi              # Create CDI for hardware
```

### Custom Project
```bash
# Include the header in your project
cp include/anim4dc.h /path/to/your/project/

# Add to your Makefile
KOS_CFLAGS += -I/path/to/anim4dc/include

# Use in your code
#define ANIM4DC_IMPLEMENTATION
#include "anim4dc.h"
```

## 📁 Project Structure

```
anim4dc/
├── include/
│   └── anim4dc.h           # Main header with implementation
├── examples/
│   └── fox_demo/           # Complete Fox model demo
│       ├── main.c          # Demo source code
│       ├── Makefile        # Build configuration
│       └── romdisk/        # Fox model and textures
├── .github/
│   └── copilot-instructions.md
└── README.md
```

## 🦊 Fox Model Demo

The Fox demo demonstrates:

- **3 Animations**: Survey (idle), Walk (movement), Run (fast movement)
- **25 Instances**: Arranged in a circle for performance testing
- **LOD Optimization**: Different animation rates based on distance
- **Real-time Stats**: FPS, memory usage, culling statistics
- **Interactive Controls**: Animation switching, camera rotation

## 💡 Memory Optimization

Designed for Dreamcast's **16MB RAM constraint**:

- **20 keyframes maximum** per animation (vs 30+ in typical systems)
- **Keyframe step optimization** (every 4th-8th frame)
- **Efficient interpolation** buffer reuse
- **LOD-based culling** to reduce active instances
- **Memory usage reporting** for optimization

## ⚡ Performance Tips

1. **Use LOD System**: Always call `Anim4dcUpdateInstanceLOD()` before rendering
2. **Limit Keyframes**: 20 keyframes provides smooth animation with minimal memory
3. **Optimize Model**: Lower poly count models perform better
4. **Batch Rendering**: Use `Anim4dcModelInstance` arrays for multiple objects
5. **Target Resolution**: 320x240 provides best performance/quality balance

## 🐛 Troubleshooting

### Model Loading Issues
- Ensure model has **skinning data** (boneIds, boneWeights, animVertices)
- Check **bone count consistency** between model and animations
- Use **multi-format fallback**: GLTF → IQM → OBJ

### Performance Issues
- Reduce instance count (25 → 12 for real hardware)
- Lower keyframe count in baking
- Enable LOD culling
- Use 640x480 resolution

### Memory Issues
- Monitor with `Anim4dcGetStats()`
- Reduce keyframe step size
- Limit active animations
- Check total memory usage < 16MB

## 🤝 Development Context

Extracted from successful **DMS_vs_anim4dc benchmark** that achieved:
- ✅ Smooth Fox animations on Dreamcast hardware  
- ✅ 25 FPS performance with 25 models on Flycast
- ✅ Memory-optimized build for real hardware
- ✅ Vertex animation system outperforming skeletal alternatives

This plugin makes the proven animation system **reusable for community developers**.

## 📄 License

MIT License - see [LICENSE](LICENSE) file for details.

## 🔗 Links

- [KallistiOS](https://github.com/KallistiOS/KallistiOS) - Dreamcast development environment
- [raylib](https://raylib.com) - Graphics library
- [GLdc](https://github.com/Kazade/GLdc) - OpenGL implementation for Dreamcast

---

**Made with ❤️ for the Dreamcast homebrew community**# anim4dc
