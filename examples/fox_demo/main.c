/**********************************************************************************************
*
*   Anim4DC Fox Demo - Vertex Animation System Showcase
*
*   This demo showcases the Anim4DC vertex animation system with the Fox model
*   featuring Survey, Walk, and Run animations optimized for Dreamcast hardware.
*
*   FEATURES DEMONSTRATED:
*       - Vertex animation baking from skeletal data
*       - Multi-format model loading (GLTF fallback chain)
*       - LOD-based performance optimization
*       - Batch rendering with 25 animated fox instances
*       - Real-time performance monitoring
*
*   CONTROLS:
*       - D-pad: Change camera angle
*       - A button: Toggle animation (Survey/Walk/Run)
*       - B button: Toggle debug information
*       - Start: Pause/unpause animation
*
*   PERFORMANCE TARGETS:
*       - Flycast: 25 FPS with 25 models, 60 FPS with 3 models
*       - Real Hardware: 25 FPS with 12 models (memory optimized)
*
**********************************************************************************************/

#define ANIM4DC_IMPLEMENTATION
#include "anim4dc.h"

#include <kos.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glkos.h>
#include <dc/maple.h>
#include <dc/maple/controller.h>
#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>
#include <dc/cdrom.h>
#include <dc/video.h>
#include <dc/biosfont.h>
#include <kos/fs_romdisk.h>

// External romdisk symbols
extern uint8 __romdisk_img[] asm("__romdisk_img");
extern uint8 __romdisk_img_end[] asm("__romdisk_img_end");

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);

// Screen dimensions optimized for Dreamcast
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define TARGET_FPS 60

// Demo configuration
#define MAX_FOX_INSTANCES 1      // Flycast target (reduce to 12 for real hardware)
#define CAMERA_DISTANCE 200.0f
#define ROTATION_SPEED 30.0f

// Use KOS controller constants directly
#define BUTTON_A CONT_A
#define BUTTON_B CONT_B
#define BUTTON_START CONT_START
#define DPAD_LEFT CONT_DPAD_LEFT
#define DPAD_RIGHT CONT_DPAD_RIGHT

// Animation names for cycling
const char *animationNames[] = {"Survey", "Walk", "Run"};
const int animationCount = 3;

// Demo state
typedef struct {
    Model foxModel;
    ModelAnimation *foxAnimations;
    int foxAnimationCount;
    
    Anim4dcModelInstance foxInstances[MAX_FOX_INSTANCES];
    int activeInstances;
    
    Camera3D camera;
    int currentAnimationIndex;
    bool showDebug;
    bool animationPaused;
    
    float globalRotation;
    float frameTime;
    float fps;
    int frameCount;
    
    bool initialized;
    char statusMessage[256];
} DemoState;

static DemoState demo = { 0 };

//------------------------------------------------------------------------------------
// Demo Functions
//------------------------------------------------------------------------------------

// Initialize fox instances in a circular pattern
void InitializeFoxInstances(void) {
    demo.activeInstances = MAX_FOX_INSTANCES;
    float radius = 80.0f;
    
    for (int i = 0; i < demo.activeInstances; i++) {
        float angle = (2.0f * PI * i) / demo.activeInstances;
        
        demo.foxInstances[i].position = (Vector3){
            cosf(angle) * radius,
            0.0f,
            sinf(angle) * radius
        };
        demo.foxInstances[i].rotation = (Vector3){ 0.0f, angle * RAD2DEG + 90.0f, 0.0f };
        demo.foxInstances[i].scale = 1.0f;
        demo.foxInstances[i].animationIndex = 0;  // Start with Survey
        demo.foxInstances[i].animationTime = (float)i * 0.1f;  // Stagger animations
        demo.foxInstances[i].lodLevel = ANIM4DC_LOD_NEAR;
        demo.foxInstances[i].visible = true;
        demo.foxInstances[i].distanceSquared = 0.0f;
    }
    
    printf("Fox Demo: Initialized %d fox instances\n", demo.activeInstances);
}

// Handle controller input
void HandleInput(void) {
    maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if (!cont) return;
    
    cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
    if (!state) return;
    
    static uint16 lastButtons = 0;
    uint16 pressed = state->buttons & ~lastButtons;
    
    // Toggle animation with A button
    if (pressed & BUTTON_A) {
        demo.currentAnimationIndex = (demo.currentAnimationIndex + 1) % animationCount;
        Anim4dcSetAnimationByName(animationNames[demo.currentAnimationIndex]);
        
        // Update all instance animations
        for (int i = 0; i < demo.activeInstances; i++) {
            demo.foxInstances[i].animationIndex = demo.currentAnimationIndex;
        }
        
        snprintf(demo.statusMessage, sizeof(demo.statusMessage), 
                "Animation: %s", animationNames[demo.currentAnimationIndex]);
        printf("Fox Demo: Switched to %s animation\n", animationNames[demo.currentAnimationIndex]);
    }
    
    // Toggle debug info with B button
    if (pressed & BUTTON_B) {
        demo.showDebug = !demo.showDebug;
    }
    
    // Pause animation with Start
    if (pressed & BUTTON_START) {
        demo.animationPaused = !demo.animationPaused;
    }
    
    // Camera rotation with D-pad
    if (state->buttons & DPAD_LEFT) {
        demo.globalRotation -= ROTATION_SPEED * GetFrameTime();
    }
    if (state->buttons & DPAD_RIGHT) {
        demo.globalRotation += ROTATION_SPEED * GetFrameTime();
    }
    
    lastButtons = state->buttons;
}

// Update camera position based on rotation
void UpdateDemoCamera(void) {
    float x = cosf(demo.globalRotation * DEG2RAD) * CAMERA_DISTANCE;
    float z = sinf(demo.globalRotation * DEG2RAD) * CAMERA_DISTANCE;
    
    demo.camera.position = (Vector3){ x, 50.0f, z + 100.0f };
    demo.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    demo.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
}

// Render debug information
void RenderDebugInfo(void) {
    if (!demo.showDebug) return;
    
    Anim4dcStats stats = Anim4dcGetStats();
    
    char debugText[512];
    snprintf(debugText, sizeof(debugText),
        "Anim4DC Fox Demo v%s\n"
        "FPS: %.1f | Instances: %d/%d\n"
        "Visible: %d | Culled: %d\n"
        "Animation: %s (%.2fs)\n"
        "Memory: %d KB\n"
        "Controls: A=Anim, B=Debug, Start=Pause",
        Anim4dcGetVersion(),
        demo.fps, demo.activeInstances, MAX_FOX_INSTANCES,
        stats.visibleInstances, stats.culledInstances,
        animationNames[demo.currentAnimationIndex],
        Anim4dcGetAnimationTime(),
        stats.memoryUsageKB
    );
    
    DrawText(debugText, 10, 10, 10, WHITE);
    
    // Draw status message
    if (strlen(demo.statusMessage) > 0) {
        DrawText(demo.statusMessage, 10, SCREEN_HEIGHT - 30, 12, YELLOW);
    }
}

//------------------------------------------------------------------------------------
// Main Program
//------------------------------------------------------------------------------------

int main(void) {
    // Mount romdisk
    fs_romdisk_mount("/rd", __romdisk_img, 1);
    
    // Initialize Raylib
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Anim4DC Fox Demo");
    SetTargetFPS(TARGET_FPS);
    
    // Initialize Anim4DC system
    if (!Anim4dcInit()) {
        printf("Fox Demo: Failed to initialize Anim4DC system\n");
        return -1;
    }
    
    // Setup camera
    demo.camera.position = (Vector3){ CAMERA_DISTANCE, 50.0f, 0.0f };
    demo.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    demo.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    demo.camera.fovy = 45.0f;
    demo.camera.projection = CAMERA_PERSPECTIVE;
    
    printf("Fox Demo: Loading Fox model with fallback chain...\n");
    
    // Load Fox model with fallback support
    demo.foxModel = Anim4dcLoadModel("/rd/Fox");
    
    if (demo.foxModel.meshCount == 0) {
        printf("Fox Demo: Failed to load Fox model\n");
        strcpy(demo.statusMessage, "ERROR: Failed to load Fox model");
    } else {
        printf("Fox Demo: Fox model loaded successfully\n");
        
        // Load animations
        demo.foxAnimations = LoadModelAnimations("/rd/Fox.gltf", &demo.foxAnimationCount);
        
        if (demo.foxAnimationCount > 0) {
            printf("Fox Demo: Loaded %d animations\n", demo.foxAnimationCount);
            
            // Bake vertex animations
            if (Anim4dcBakeVertexAnimations(demo.foxModel, demo.foxAnimations, demo.foxAnimationCount)) {
                printf("Fox Demo: Vertex animations baked successfully\n");
                InitializeFoxInstances();
                demo.initialized = true;
                strcpy(demo.statusMessage, "Fox Demo Ready - Press A to change animation");
            } else {
                printf("Fox Demo: Failed to bake vertex animations\n");
                strcpy(demo.statusMessage, "ERROR: Animation baking failed");
            }
        } else {
            printf("Fox Demo: No animations found\n");
            strcpy(demo.statusMessage, "ERROR: No animations found in model");
        }
    }
    
    // Main loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        demo.frameTime += deltaTime;
        demo.frameCount++;
        
        // Calculate FPS every second
        if (demo.frameTime >= 1.0f) {
            demo.fps = demo.frameCount / demo.frameTime;
            demo.frameTime = 0.0f;
            demo.frameCount = 0;
        }
        
        // Handle input
        HandleInput();
        
        // Update camera
        UpdateDemoCamera();
        
        // Update animations
        if (demo.initialized && !demo.animationPaused) {
            Anim4dcUpdateAnimation(deltaTime);
            
            // Update LOD for all instances
            Anim4dcUpdateInstanceLOD(demo.foxInstances, demo.activeInstances, demo.camera.position);
        }
        
        // Render
        BeginDrawing();
        ClearBackground(SKYBLUE);
        
        BeginMode3D(demo.camera);
        
        // Draw ground grid
        DrawGrid(20, 10.0f);
        
        if (demo.initialized) {
            // Update model vertices with current animation frame
            float *interpolatedVertices = Anim4dcGetInterpolatedVertices();
            if (interpolatedVertices && demo.foxModel.meshCount > 0) {
                // Copy interpolated vertices to model
                memcpy(demo.foxModel.meshes[0].vertices, interpolatedVertices,
                       demo.foxModel.meshes[0].vertexCount * 3 * sizeof(float));
                UploadMesh(&demo.foxModel.meshes[0], false);
            }
            
            // Render all fox instances
            for (int i = 0; i < demo.activeInstances; i++) {
                if (demo.foxInstances[i].visible) {
                    Vector3 pos = demo.foxInstances[i].position;
                    Vector3 rot = demo.foxInstances[i].rotation;
                    float scale = demo.foxInstances[i].scale;
                    
                    // Create transformation matrix
                    Matrix transform = MatrixIdentity();
                    transform = MatrixMultiply(transform, MatrixScale(scale, scale, scale));
                    transform = MatrixMultiply(transform, MatrixRotateXYZ((Vector3){
                        rot.x * DEG2RAD, rot.y * DEG2RAD, rot.z * DEG2RAD
                    }));
                    transform = MatrixMultiply(transform, MatrixTranslate(pos.x, pos.y, pos.z));
                    
                    // Color based on LOD level
                    Color modelColor = WHITE;
                    switch (demo.foxInstances[i].lodLevel) {
                        case ANIM4DC_LOD_NEAR: modelColor = WHITE; break;
                        case ANIM4DC_LOD_MID: modelColor = LIGHTGRAY; break;
                        case ANIM4DC_LOD_FAR: modelColor = GRAY; break;
                        default: modelColor = DARKGRAY; break;
                    }
                    
                    DrawModel(demo.foxModel, pos, scale, modelColor);
                }
            }
        }
        
        EndMode3D();
        
        // Render UI
        RenderDebugInfo();
        
        EndDrawing();
    }
    
    // Cleanup
    if (demo.foxAnimationCount > 0) {
        UnloadModelAnimations(demo.foxAnimations, demo.foxAnimationCount);
    }
    UnloadModel(demo.foxModel);
    
    Anim4dcShutdown();  
    CloseWindow();
    
    return 0;
}