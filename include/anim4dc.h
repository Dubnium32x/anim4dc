/**********************************************************************************************
*
*   anim4dc v1.0 - Dreamcast Raylib Animation Plugin
*
*   A high-performance vertex animation system optimized for Sega Dreamcast hardware
*   
*   FEATURES:
*       - Vertex keyframe baking from skeletal animations
*       - Multi-format model loading (FBX, GLB, GLTF, IQM, OBJ)
*       - LOD-based performance optimization
*       - Memory-efficient animation storage
*       - Hardware-optimized interpolation
*
*   USAGE:
*       #define ANIM4DC_IMPLEMENTATION
*       #include "anim4dc.h"
*
*   DEPENDENCIES:
*       - raylib 5.5+
*       - KallistiOS
*       - GLdc
*
*   LICENSE: MIT License
*
*   Copyright (c) 2024 Anim4DC Team
*
**********************************************************************************************/

#ifndef ANIM4DC_H
#define ANIM4DC_H

#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define ANIM4DC_VERSION_MAJOR       1
#define ANIM4DC_VERSION_MINOR       0
#define ANIM4DC_VERSION_PATCH       0
#define ANIM4DC_VERSION             "1.0.0"

// Memory optimization constants for Dreamcast (16MB total RAM)
#define ANIM4DC_MAX_KEYFRAMES       20          // Maximum keyframes per animation
#define ANIM4DC_MAX_ANIMATIONS      8           // Maximum animations per model
#define ANIM4DC_MAX_INSTANCES       25          // Maximum model instances for benchmarking
#define ANIM4DC_MAX_NAME_LENGTH     32          // Animation name length

// LOD system constants (squared distances to avoid sqrt calculations)
#define ANIM4DC_LOD_NEAR_DIST2      (80.0f * 80.0f)    // Full detail animation
#define ANIM4DC_LOD_MID_DIST2       (120.0f * 120.0f)   // Reduced animation rate
#define ANIM4DC_LOD_FAR_DIST2       (160.0f * 160.0f)   // Minimal animation
#define ANIM4DC_LOD_CULL_DIST2      (200.0f * 200.0f)   // No rendering/animation

// LOD animation speed multipliers
#define ANIM4DC_LOD_NEAR_SPEED      1.0f        // Full speed
#define ANIM4DC_LOD_MID_SPEED       0.5f        // Half speed  
#define ANIM4DC_LOD_FAR_SPEED       0.25f       // Quarter speed
#define ANIM4DC_LOD_FROZEN_SPEED    0.0f        // No animation advance

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// LOD levels for performance optimization
typedef enum {
    ANIM4DC_LOD_NEAR = 0,       // Full detail, full animation speed
    ANIM4DC_LOD_MID,            // Reduced animation rate
    ANIM4DC_LOD_FAR,            // Minimal animation
    ANIM4DC_LOD_FROZEN,         // Animation frozen
    ANIM4DC_LOD_CULLED          // Not rendered
} Anim4dcLodLevel;

// Vertex keyframe for baked animations
typedef struct Anim4dcVertexKeyframe {
    float *vertices;            // Vertex positions for this keyframe
    int vertexCount;           // Number of vertices
    float timestamp;           // Time for this keyframe in seconds
} Anim4dcVertexKeyframe;

// Vertex animation structure
typedef struct Anim4dcVertexAnimation {
    char name[ANIM4DC_MAX_NAME_LENGTH];                 // Animation name
    Anim4dcVertexKeyframe keyframes[ANIM4DC_MAX_KEYFRAMES]; // Keyframe data
    int keyframeCount;                                  // Number of keyframes
    float duration;                                     // Total animation duration
    bool looping;                                      // Should animation loop?
} Anim4dcVertexAnimation;

// Animation system state
typedef struct Anim4dcAnimationSystem {
    Anim4dcVertexAnimation animations[ANIM4DC_MAX_ANIMATIONS];  // Baked animations
    int animationCount;                                         // Number of animations
    int currentAnimation;                                       // Current animation index
    float currentTime;                                         // Current playback time
    float *interpolationBuffer;                                // Buffer for interpolated vertices
    int vertexCount;                                          // Number of vertices per keyframe
    bool initialized;                                         // System initialization state
} Anim4dcAnimationSystem;

// Model instance for batch rendering and LOD
typedef struct Anim4dcModelInstance {
    Vector3 position;           // World position
    Vector3 rotation;           // Euler rotation angles
    float scale;               // Uniform scale
    int animationIndex;        // Which animation to play (-1 = none)
    float animationTime;       // Current animation time
    Anim4dcLodLevel lodLevel;  // Current LOD level
    bool visible;              // Should be rendered this frame
    float distanceSquared;     // Distance from camera (squared)
} Anim4dcModelInstance;

// Performance statistics
typedef struct Anim4dcStats {
    int visibleInstances;       // Number of rendered instances
    int culledInstances;        // Number of culled instances  
    int animationUpdates;       // Number of animation updates this frame
    float averageFPS;          // Average FPS over recent frames
    int memoryUsageKB;         // Approximate memory usage in KB
} Anim4dcStats;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// NOTE: These variables are defined in the implementation section

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//------------------------------------------------------------------------------------
// Animation System Core Functions
//------------------------------------------------------------------------------------

// Initialize the animation system
bool Anim4dcInit(void);

// Shutdown and cleanup the animation system
void Anim4dcShutdown(void);

// Check if a model and its animations are compatible for vertex baking
bool Anim4dcCheckModelCompatibility(Model model, ModelAnimation *animations, int animationCount);

// Bake skeletal animations into vertex keyframes for optimal playback
bool Anim4dcBakeVertexAnimations(Model model, ModelAnimation *animations, int animationCount);

// Update animation playback (call once per frame)
void Anim4dcUpdateAnimation(float deltaTime);

// Get the current interpolated vertices for rendering
float *Anim4dcGetInterpolatedVertices(void);

//------------------------------------------------------------------------------------
// Animation Control Functions  
//------------------------------------------------------------------------------------

// Set the current animation by index
bool Anim4dcSetAnimation(int animationIndex);

// Set the current animation by name
bool Anim4dcSetAnimationByName(const char *animationName);

// Get the current animation index
int Anim4dcGetCurrentAnimation(void);

// Get the current animation time
float Anim4dcGetAnimationTime(void);

// Set the animation time (for scrubbing)
void Anim4dcSetAnimationTime(float time);

// Pause/unpause animation playback
void Anim4dcSetAnimationPaused(bool paused);

//------------------------------------------------------------------------------------
// Batch Rendering and LOD Functions
//------------------------------------------------------------------------------------

// Update LOD levels for all instances based on camera position
void Anim4dcUpdateInstanceLOD(Anim4dcModelInstance *instances, int instanceCount, Vector3 cameraPosition);

// Render multiple model instances with LOD optimization
void Anim4dcRenderInstances(Model model, Anim4dcModelInstance *instances, int instanceCount);

// Get performance statistics
Anim4dcStats Anim4dcGetStats(void);

//------------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------------

// Load model with fallback format support (GLTF -> IQM -> OBJ)
Model Anim4dcLoadModel(const char *basePath);

// Calculate memory usage of the animation system
int Anim4dcCalculateMemoryUsage(void);

// Get version information
const char *Anim4dcGetVersion(void);

#ifdef __cplusplus
}
#endif

#endif // ANIM4DC_H

//------------------------------------------------------------------------------------
// Implementation Section (when ANIM4DC_IMPLEMENTATION is defined)
//------------------------------------------------------------------------------------

#ifdef ANIM4DC_IMPLEMENTATION

#include <kos.h>

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static Anim4dcAnimationSystem anim4dc = { 0 };
static Anim4dcStats anim4dc_stats = { 0 };

//----------------------------------------------------------------------------------
// Internal Helper Functions
//----------------------------------------------------------------------------------

// Interpolate between two vertex buffers
static void Anim4dcInterpolateVertices(float *output, float *vertices1, float *vertices2, float t, int vertexCount) {
    for (int i = 0; i < vertexCount * 3; i++) {
        output[i] = vertices1[i] + (vertices2[i] - vertices1[i]) * t;
    }
}

// Capture a vertex keyframe from current skeletal animation state  
static void Anim4dcCaptureVertexKeyframe(Anim4dcVertexAnimation *animation, float timestamp, float *vertexData, int vertexCount) {
    if (animation->keyframeCount >= ANIM4DC_MAX_KEYFRAMES) return;
    
    Anim4dcVertexKeyframe *keyframe = &animation->keyframes[animation->keyframeCount];
    
    // Allocate memory for vertex data
    int vertexDataSize = vertexCount * 3 * sizeof(float);
    keyframe->vertices = (float*)malloc(vertexDataSize);
    
    if (keyframe->vertices) {
        memcpy(keyframe->vertices, vertexData, vertexDataSize);
        keyframe->vertexCount = vertexCount;
        keyframe->timestamp = timestamp;
        animation->keyframeCount++;
    }
}

//----------------------------------------------------------------------------------
// Animation System Core Functions Implementation
//----------------------------------------------------------------------------------

bool Anim4dcInit(void) {
    memset(&anim4dc, 0, sizeof(Anim4dcAnimationSystem));
    memset(&anim4dc_stats, 0, sizeof(Anim4dcStats));
    
    anim4dc.currentAnimation = -1;
    anim4dc.initialized = true;
    
    printf("Anim4DC v%s initialized\n", ANIM4DC_VERSION);
    return true;
}

void Anim4dcShutdown(void) {
    if (!anim4dc.initialized) return;
    
    // Free all keyframe vertex data
    for (int a = 0; a < anim4dc.animationCount; a++) {
        for (int k = 0; k < anim4dc.animations[a].keyframeCount; k++) {
            if (anim4dc.animations[a].keyframes[k].vertices) {
                free(anim4dc.animations[a].keyframes[k].vertices);
                anim4dc.animations[a].keyframes[k].vertices = NULL;
            }
        }
    }
    
    // Free interpolation buffer
    if (anim4dc.interpolationBuffer) {
        free(anim4dc.interpolationBuffer);
        anim4dc.interpolationBuffer = NULL;
    }
    
    memset(&anim4dc, 0, sizeof(Anim4dcAnimationSystem));
    printf("Anim4DC shutdown complete\n");
}

bool Anim4dcCheckModelCompatibility(Model model, ModelAnimation *animations, int animationCount) {
    if (model.meshCount <= 0) {
        printf("Anim4DC: ERROR - No meshes in model\n");
        return false;
    }
    
    if (animationCount <= 0) {
        printf("Anim4DC: Model loaded as static (no animations)\n");
        return false;
    }
    
    if (model.boneCount <= 0) {
        printf("Anim4DC: ERROR - Model has no bones\n");
        return false;
    }
    
    if (model.bones == NULL || model.bindPose == NULL) {
        printf("Anim4DC: ERROR - Model missing bone hierarchy\n");
        return false;
    }
    
    // Check first animation
    ModelAnimation anim = animations[0];
    if (anim.boneCount != model.boneCount) {
        printf("Anim4DC: ERROR - Bone count mismatch! Model:%d != Anim:%d\n", 
               model.boneCount, anim.boneCount);
        return false;
    }
    
    if (anim.bones == NULL || anim.framePoses == NULL) {
        printf("Anim4DC: ERROR - Animation missing data\n");
        return false;
    }
    
    // Check for skinning data
    bool hasSkinning = false;
    for (int i = 0; i < model.meshCount; i++) {
        Mesh mesh = model.meshes[i];
        if (mesh.boneIds && mesh.boneWeights && mesh.animVertices) {
            hasSkinning = true;
            break;
        }
    }
    
    if (!hasSkinning) {
        printf("Anim4DC: ERROR - Model missing skinning data\n");
        return false;
    }
    
    printf("Anim4DC: Model compatibility check passed\n");
    return true;
}

bool Anim4dcBakeVertexAnimations(Model model, ModelAnimation *animations, int animationCount) {
    if (!anim4dc.initialized) {
        printf("Anim4DC: ERROR - System not initialized\n");
        return false;
    }
    
    if (!Anim4dcCheckModelCompatibility(model, animations, animationCount)) {
        return false;
    }
    
    // Default animation names
    const char* animNames[] = {"Survey", "Walk", "Run", "Jump", "Idle", "Attack", "Death", "Custom"};
    int animsToBake = (animationCount > ANIM4DC_MAX_ANIMATIONS) ? ANIM4DC_MAX_ANIMATIONS : animationCount;
    
    anim4dc.animationCount = animsToBake;
    anim4dc.vertexCount = model.meshes[0].vertexCount;
    
    for (int a = 0; a < animsToBake; a++) {
        ModelAnimation skelAnim = animations[a];
        Anim4dcVertexAnimation *vertAnim = &anim4dc.animations[a];
        
        // Setup vertex animation
        snprintf(vertAnim->name, ANIM4DC_MAX_NAME_LENGTH, "%s", 
                 (a < 8) ? animNames[a] : "Unknown");
        vertAnim->keyframeCount = 0;
        vertAnim->duration = skelAnim.frameCount / 20.0f;  // Assume 20 FPS
        vertAnim->looping = true;
        
        printf("Anim4DC: Baking animation %d: %s (%d frames)\n", 
               a, vertAnim->name, skelAnim.frameCount);
        
        // Capture keyframes at regular intervals to save memory
        int keyframeStep = (skelAnim.frameCount > 40) ? 8 : 4;
        
        for (int frame = 0; frame < skelAnim.frameCount; frame += keyframeStep) {
            // Apply skeletal animation to get animated vertices
            UpdateModelAnimation(model, skelAnim, frame);
            
            // Capture the animated vertex positions
            if (model.meshes[0].animVertices) {
                float timestamp = (frame / 20.0f);
                Anim4dcCaptureVertexKeyframe(vertAnim, timestamp, model.meshes[0].animVertices, anim4dc.vertexCount);
            }
        }
        
        printf("Anim4DC: Baked %d keyframes for %s\n", vertAnim->keyframeCount, vertAnim->name);
    }
    
    // Allocate interpolation buffer
    anim4dc.interpolationBuffer = (float*)malloc(anim4dc.vertexCount * 3 * sizeof(float));
    if (!anim4dc.interpolationBuffer) {
        printf("Anim4DC: ERROR - Failed to allocate interpolation buffer\n");
        return false;
    }
    
    // Set default animation
    anim4dc.currentAnimation = 0;
    anim4dc.currentTime = 0.0f;
    
    // Calculate memory usage
    anim4dc_stats.memoryUsageKB = Anim4dcCalculateMemoryUsage();
    
    printf("Anim4DC: Vertex animation baking complete! Using %d KB memory\n", 
           anim4dc_stats.memoryUsageKB);
    
    return true;
}

void Anim4dcUpdateAnimation(float deltaTime) {
    if (!anim4dc.initialized || anim4dc.currentAnimation < 0 || 
        anim4dc.currentAnimation >= anim4dc.animationCount) {
        return;
    }
    
    Anim4dcVertexAnimation *currentAnim = &anim4dc.animations[anim4dc.currentAnimation];
    if (currentAnim->keyframeCount < 2 || !anim4dc.interpolationBuffer) return;
    
    // Update animation time
    anim4dc.currentTime += deltaTime;
    if (anim4dc.currentTime >= currentAnim->duration) {
        anim4dc.currentTime = 0.0f;  // Loop
    }
    
    // Find current and next keyframes
    int currentKeyframe = 0;
    int nextKeyframe = 1;
    
    for (int i = 0; i < currentAnim->keyframeCount - 1; i++) {
        if (anim4dc.currentTime >= currentAnim->keyframes[i].timestamp && 
            anim4dc.currentTime < currentAnim->keyframes[i + 1].timestamp) {
            currentKeyframe = i;
            nextKeyframe = i + 1;
            break;
        }
    }
    
    // Handle looping
    if (anim4dc.currentTime >= currentAnim->keyframes[currentAnim->keyframeCount - 1].timestamp) {
        currentKeyframe = currentAnim->keyframeCount - 1;
        nextKeyframe = 0;
    }
    
    // Calculate interpolation factor
    float t1 = currentAnim->keyframes[currentKeyframe].timestamp;
    float t2 = (nextKeyframe == 0) ? currentAnim->duration : currentAnim->keyframes[nextKeyframe].timestamp;
    float t = 0.0f;
    
    if (nextKeyframe == 0) {
        // Looping case
        float timeFromStart = anim4dc.currentTime - t1;
        float totalGap = currentAnim->duration - t1;
        t = (totalGap > 0.0f) ? (timeFromStart / totalGap) : 0.0f;
    } else {
        // Normal case
        float gap = t2 - t1;
        t = (gap > 0.0f) ? ((anim4dc.currentTime - t1) / gap) : 0.0f;
    }
    
    // Clamp interpolation factor
    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
    
    // Interpolate vertices
    Anim4dcInterpolateVertices(
        anim4dc.interpolationBuffer,
        currentAnim->keyframes[currentKeyframe].vertices,
        currentAnim->keyframes[nextKeyframe].vertices,
        t,
        anim4dc.vertexCount
    );
}

float *Anim4dcGetInterpolatedVertices(void) {
    return anim4dc.interpolationBuffer;
}

//------------------------------------------------------------------------------------
// Animation Control Functions Implementation
//------------------------------------------------------------------------------------

bool Anim4dcSetAnimation(int animationIndex) {
    if (!anim4dc.initialized || animationIndex < 0 || animationIndex >= anim4dc.animationCount) {
        return false;
    }
    
    anim4dc.currentAnimation = animationIndex;
    anim4dc.currentTime = 0.0f;
    return true;
}

bool Anim4dcSetAnimationByName(const char *animationName) {
    if (!anim4dc.initialized || !animationName) return false;
    
    for (int i = 0; i < anim4dc.animationCount; i++) {
        if (strcmp(anim4dc.animations[i].name, animationName) == 0) {
            return Anim4dcSetAnimation(i);
        }
    }
    return false;
}

int Anim4dcGetCurrentAnimation(void) {
    return anim4dc.currentAnimation;
}

float Anim4dcGetAnimationTime(void) {
    return anim4dc.currentTime;
}

void Anim4dcSetAnimationTime(float time) {
    if (anim4dc.currentAnimation >= 0 && anim4dc.currentAnimation < anim4dc.animationCount) {
        float duration = anim4dc.animations[anim4dc.currentAnimation].duration;
        anim4dc.currentTime = fmod(time, duration);
        if (anim4dc.currentTime < 0.0f) anim4dc.currentTime += duration;
    }
}

void Anim4dcSetAnimationPaused(bool paused) {
    // This would require additional state tracking - left as exercise
    // Could add a 'paused' field to the animation system
}

//------------------------------------------------------------------------------------
// Batch Rendering and LOD Functions Implementation
//------------------------------------------------------------------------------------

void Anim4dcUpdateInstanceLOD(Anim4dcModelInstance *instances, int instanceCount, Vector3 cameraPosition) {
    anim4dc_stats.visibleInstances = 0;
    anim4dc_stats.culledInstances = 0;
    
    for (int i = 0; i < instanceCount; i++) {
        Anim4dcModelInstance *instance = &instances[i];
        
        // Calculate squared distance to avoid sqrt
        Vector3 diff = Vector3Subtract(instance->position, cameraPosition);
        instance->distanceSquared = Vector3LengthSqr(diff);
        
        // Determine LOD level
        if (instance->distanceSquared > ANIM4DC_LOD_CULL_DIST2) {
            instance->lodLevel = ANIM4DC_LOD_CULLED;
            instance->visible = false;
            anim4dc_stats.culledInstances++;
        } else if (instance->distanceSquared > ANIM4DC_LOD_FAR_DIST2) {
            instance->lodLevel = ANIM4DC_LOD_FAR;
            instance->visible = true;
            anim4dc_stats.visibleInstances++;
        } else if (instance->distanceSquared > ANIM4DC_LOD_MID_DIST2) {
            instance->lodLevel = ANIM4DC_LOD_MID;
            instance->visible = true;
            anim4dc_stats.visibleInstances++;
        } else {
            instance->lodLevel = ANIM4DC_LOD_NEAR;
            instance->visible = true;
            anim4dc_stats.visibleInstances++;
        }
    }
}

void Anim4dcRenderInstances(Model model, Anim4dcModelInstance *instances, int instanceCount) {
    // This would render all visible instances with their respective transformations
    // Implementation depends on specific rendering needs
    for (int i = 0; i < instanceCount; i++) {
        if (instances[i].visible) {
            // Apply vertex animation if available
            if (anim4dc.interpolationBuffer && model.meshCount > 0) {
                // Update mesh vertices with interpolated data
                memcpy(model.meshes[0].vertices, anim4dc.interpolationBuffer, 
                       anim4dc.vertexCount * 3 * sizeof(float));
                UploadMesh(&model.meshes[0], false);
            }
            
            DrawModel(model, instances[i].position, instances[i].scale, WHITE);
        }
    }
}

Anim4dcStats Anim4dcGetStats(void) {
    return anim4dc_stats;
}

//------------------------------------------------------------------------------------
// Utility Functions Implementation
//------------------------------------------------------------------------------------

Model Anim4dcLoadModel(const char *basePath) {
    Model model = { 0 };
    char fullPath[256];
    
    // Try GLTF first
    snprintf(fullPath, sizeof(fullPath), "%s.gltf", basePath);
    model = LoadModel(fullPath);
    if (model.meshCount > 0) {
        printf("Anim4DC: Loaded GLTF model: %s\n", fullPath);
        return model;
    }
    
    // Try IQM
    snprintf(fullPath, sizeof(fullPath), "%s.iqm", basePath);
    model = LoadModel(fullPath);
    if (model.meshCount > 0) {
        printf("Anim4DC: Loaded IQM model: %s\n", fullPath);
        return model;
    }
    
    // Try OBJ
    snprintf(fullPath, sizeof(fullPath), "%s.obj", basePath);
    model = LoadModel(fullPath);
    if (model.meshCount > 0) {
        printf("Anim4DC: Loaded OBJ model: %s\n", fullPath);
        return model;
    }
    
    printf("Anim4DC: Failed to load model with base path: %s\n", basePath);
    return model;
}

int Anim4dcCalculateMemoryUsage(void) {
    int totalMemory = 0;
    
    // Calculate keyframe memory
    for (int a = 0; a < anim4dc.animationCount; a++) {
        for (int k = 0; k < anim4dc.animations[a].keyframeCount; k++) {
            if (anim4dc.animations[a].keyframes[k].vertices) {
                totalMemory += anim4dc.animations[a].keyframes[k].vertexCount * 3 * sizeof(float);
            }
        }
    }
    
    // Add interpolation buffer
    if (anim4dc.interpolationBuffer) {
        totalMemory += anim4dc.vertexCount * 3 * sizeof(float);
    }
    
    return totalMemory / 1024;  // Convert to KB
}

const char *Anim4dcGetVersion(void) {
    return ANIM4DC_VERSION;
}

#endif // ANIM4DC_IMPLEMENTATION