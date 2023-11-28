#include <stdio.h>

#include "common.h"

int main(int argc, char *argv[])
{
    const int screenWidth = 800;
    const int screenHeight = 400;
    InitWindow(screenWidth, screenHeight, "Particle Emitter");

    // Define the camera to look into our 3d world
    Camera camera = {0};
    camera.position = (Vector3){0, 5, 15};  // Camera position
    camera.target = (Vector3){0, 0, 0};      // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                     // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;  // Camera projection type

    EmitConfiguration configuration;
    configuration.particlesPerSecond = 10;
    configuration.lifespan = (Range){.min = 4, .max = 4};
    configuration.speed = (Range){.min = 2, .max = 3};
    configuration.color.r = (InterpolationRange){.start = (Range){.min = 0, .max = 0}, .end = (Range){.min = 0.1, .max = 0.1}};
    configuration.color.g = (InterpolationRange){.start = (Range){.min = 0.8, .max = 0.8}, .end = (Range){.min = 1, .max = 1}};
    configuration.color.b = (InterpolationRange){.start = (Range){.min = 0, .max = 0}, .end = (Range){.min = 0.1, .max = 0.1}};
    configuration.color.a = (InterpolationRange){.start = (Range){.min = 1, .max = 1}, .end = (Range){.min = 0, .max = .1}};
    configuration.scale = (InterpolationRange){.start = (Range){.min = .3, .max = .3}, .end = (Range){.min = .6, .max = .6}};
    configuration.rotation = (InterpolationRange){.start = (Range){.min = 1, .max = 1}, .end = (Range){.min = 1, .max = 1}};
    Emitter emitter = particle_emitter_create(configuration);

    emitter.environment.friction.x = 0.99999;
    emitter.environment.friction.y = 0.99999;
    emitter.environment.friction.z = 0.99999;
    emitter.environment.speedMax = (Vector3d){.x = 10, .y =10, .z = 10};

    emitter.shape.type = ST_Point;
    emitter.shape.start = (Vector3d){.x = 0, .y = 0, .z = 0};

    emitter.environment.vortices = PE_CREATE_ARRAY(Vortex, 2);
    PE_INSERT(emitter.environment.vortices, ((Vortex){.pos = (Vector3d){.x = 5, .y = 0, .z = 0}, .magnitude = .005}));
    PE_INSERT(emitter.environment.vortices, ((Vortex){.pos = (Vector3d){.x = -5, .y = 0, .z = 0}, .magnitude = -.001}));

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        UpdateCamera(&camera, CAMERA_THIRD_PERSON);
        float dt = GetFrameTime();
        particle_emitter_update(&emitter, dt);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        DrawGrid(10, 1);
        LinkedList *current = emitter.first;
        while (current != NULL)
        {

            Color color = (Color){
                .r = current->item.color.r.value * 255,
                .g = current->item.color.g.value * 255,
                .b = current->item.color.b.value * 255,
                .a = current->item.color.a.value * 255};

            DrawCircle3D(
                (Vector3){.x = current->item.pos.x,
                          .y = current->item.pos.y,
                          .z = current->item.pos.z},
                current->item.scale.value,
                (Vector3){.x = 0, .y = 0, .z = 0},
                0, color);

            current = current->next;
        }

        for (size_t i = 0; i < emitter.environment.vortices.count; i++)
        {
            Vector3d c = emitter.environment.vortices.items[i].pos;
            float s = emitter.environment.vortices.items[i].magnitude;
            DrawCircle3D(
                (Vector3){.x = c.x, .y = c.y, .z = c.z},
                s,
                (Vector3){.x = 0, .y = 0, .z = 0},
                0, RED);
        }

        EndMode3D();

        char label_fps[256];
        sprintf(label_fps, "FPS: %d", GetFPS());
        DrawText(label_fps, 10, 10, 20, GRAY);
        EndDrawing();
    }

    particle_emitter_destroy(&emitter);
    CloseWindow();

    return 0;
}
