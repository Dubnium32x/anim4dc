/*
    Basic Anim4DC Example
    
    This minimal example shows how to use Anim4DC in your own projects.
    It demonstrates loading a model, baking animations, and basic playback.
*/

#define ANIM4DC_IMPLEMENTATION
#include "anim4dc.h"

#include <kos.h>
#include <kos/fs_romdisk.h>

// External romdisk symbols (if using romdisk)
extern uint8 __romdisk_img[] asm("__romdisk_img");
extern uint8 __romdisk_img_end[] asm("__romdisk_img_end");

KOS_INIT_FLAGS(INIT_DEFAULT);

int main(void) {
    // Mount romdisk (if using)
    fs_romdisk_mount("/rd", __romdisk_img, 1);
    
    // Initialize Raylib
    InitWindow(320, 240, "Basic Anim4DC Example");
    SetTargetFPS(25);
    
    // Initialize Anim4DC
    if (!Anim4dcInit()) {
        printf("Failed to initialize Anim4DC\n");
        return -1;
    }
    
    // Load your model (supports GLTF, IQM, OBJ fallback)
    Model myModel = Anim4dcLoadModel("/rd/MyModel");
    
    if (myModel.meshCount == 0) {
        printf("Failed to load model\n");
        Anim4dcShutdown();
        CloseWindow();
        return -1;
    }
    
    // Load animations
    int animCount;
    ModelAnimation *animations = LoadModelAnimations("/rd/MyModel.gltf", &animCount);
    
    if (animCount > 0) {
        // Bake skeletal animations to vertex keyframes
        if (Anim4dcBakeVertexAnimations(myModel, animations, animCount)) {
            printf("Successfully baked %d animations\n", animCount);
        } else {
            printf("Failed to bake animations\n");
        }
    }
    
    // Setup camera
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    // Main loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Update animation
        Anim4dcUpdateAnimation(deltaTime);
        
        // Get interpolated vertices
        float *animatedVertices = Anim4dcGetInterpolatedVertices();
        
        // Update model mesh with animated vertices (if available)
        if (animatedVertices && myModel.meshCount > 0) {
            memcpy(myModel.meshes[0].vertices, animatedVertices,
                   myModel.meshes[0].vertexCount * 3 * sizeof(float));
            UploadMesh(&myModel.meshes[0], false);
        }
        
        // Render
        BeginDrawing();
        ClearBackground(SKYBLUE);
        
        BeginMode3D(camera);
        
        // Draw your animated model
        DrawModel(myModel, (Vector3){ 0, 0, 0 }, 1.0f, WHITE);
        
        // Draw ground for reference
        DrawGrid(10, 1.0f);
        
        EndMode3D();
        
        // Draw UI
        DrawText("Basic Anim4DC Example", 10, 10, 16, WHITE);
        DrawFPS(10, 30);
        
        // Show current animation info
        int currentAnim = Anim4dcGetCurrentAnimation();
        float animTime = Anim4dcGetAnimationTime();
        DrawText(TextFormat("Animation: %d | Time: %.2f", currentAnim, animTime), 10, 50, 12, WHITE);
        
        EndDrawing();
    }
    
    // Cleanup
    if (animCount > 0) {
        UnloadModelAnimations(animations, animCount);
    }
    UnloadModel(myModel);
    
    Anim4dcShutdown();
    CloseWindow();
    
    return 0;
}