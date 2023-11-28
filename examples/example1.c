#include <stdio.h>

#include "common.h"

void change_shape(Emitter *emitter)
{

    if (IsKeyPressed(KEY_KP_1))
    {
        emitter->shape.type = ST_Point;
        emitter->shape.start = (Vector3d){.x = 0, .y = 10, .z = 0};
        emitter->shape.end = (Vector3d){.x = 0, .y = 0, .z = 0};
    }
    else if (IsKeyPressed(KEY_KP_2))
    {
        emitter->shape.type = ST_Line;
        emitter->shape.start = (Vector3d){.x = -5, .y = 10, .z = -5};
        emitter->shape.end = (Vector3d){.x = 5, .y = 10, .z = 5};
    }
    else if (IsKeyPressed(KEY_KP_3))
    {
        emitter->shape.type = ST_Cube;
        emitter->shape.start = (Vector3d){.x = -5, .y = 10, .z = -5};
        emitter->shape.end = (Vector3d){.x = 5, .y = 10, .z = 5};
    }
    else if (IsKeyPressed(KEY_KP_4))
    {
        emitter->shape.type = ST_Sphere;
        emitter->shape.start = (Vector3d){.x = 0, .y = 10, .z = 0};
        emitter->shape.end = (Vector3d){.x = 10, .y = 10, .z = 10};
    }
}

int main(int argc, char *argv[])
{
    const int screenWidth = 800;
    const int screenHeight = 400;
    InitWindow(screenWidth, screenHeight, "Particle Emitter");

    // Define the camera to look into our 3d world
    Camera camera = {0};
    camera.position = (Vector3){0, 20, 20};  // Camera position
    camera.target = (Vector3){0, 0, 0};      // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                     // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;  // Camera projection type

    EmitConfiguration configuration;
    configuration.particlesPerSecond = 10;
    configuration.lifespan = (Range){.min = 5, .max = 5};
    configuration.speed = (Range){.min = 0, .max = 0};
    configuration.color.r = (InterpolationRange){.start = (Range){.min = 0, .max = 0}, .end = (Range){.min = 0.1, .max = 0.1}};
    configuration.color.g = (InterpolationRange){.start = (Range){.min = 0.8, .max = 0.8}, .end = (Range){.min = 1, .max = 1}};
    configuration.color.b = (InterpolationRange){.start = (Range){.min = 0, .max = 0}, .end = (Range){.min = 0.1, .max = 0.1}};
    configuration.color.a = (InterpolationRange){.start = (Range){.min = 1, .max = 1}, .end = (Range){.min = 0, .max = .1}};
    configuration.scale = (InterpolationRange){.start = (Range){.min = .3, .max = .3}, .end = (Range){.min = .6, .max = .6}};
    configuration.rotation = (InterpolationRange){.start = (Range){.min = 1, .max = 1}, .end = (Range){.min = 1, .max = 1}};
    Emitter emitter = particle_emitter_create(configuration);

    emitter.shape.type = ST_Cube;
    emitter.shape.start = (Vector3d){.x = -5, .y = 10, .z = -5};
    emitter.shape.end = (Vector3d){.x = 5, .y = 10, .z = 5};

    emitter.environment.friction.x = 0.99999;
    emitter.environment.friction.y = 0.99999;
    emitter.environment.friction.z = 0.99999;

    emitter.environment.speedMax = (Vector3d){.x = 2, .y = 2, .z = 2};

    emitter.environment.forces = PE_CREATE_ARRAY(Force, 1);
    PE_INSERT(emitter.environment.forces, ((Force){.dir = (Vector3d){.x = 0, .y = -1, .z = 0}, .magnitude = .1}));

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        change_shape(&emitter);
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

        if (emitter.shape.type == ST_Point)
        {
            DrawPoint3D((Vector3){.x = emitter.shape.start.x, .y = emitter.shape.start.y, .z = emitter.shape.start.z}, BLUE);
        }
        else if (emitter.shape.type == ST_Line)
        {
            DrawLine3D(
                (Vector3){.x = emitter.shape.start.x, .y = emitter.shape.start.y, .z = emitter.shape.start.z},
                (Vector3){.x = emitter.shape.end.x, .y = emitter.shape.end.y, .z = emitter.shape.end.z},
                BLUE);
        }
        else if (emitter.shape.type == ST_Cube)
        {
            float w = (emitter.shape.end.x - emitter.shape.start.x);
            float h = (emitter.shape.end.y - emitter.shape.start.y);
            float l = (emitter.shape.end.z - emitter.shape.start.z);

            DrawLine3D(
                (Vector3){.x = emitter.shape.start.x, .y = emitter.shape.start.y, .z = emitter.shape.start.z},
                (Vector3){.x = emitter.shape.start.x + w, .y = emitter.shape.start.y, .z = emitter.shape.start.z},
                BLUE);
            DrawLine3D(
                (Vector3){.x = emitter.shape.start.x + w, .y = emitter.shape.start.y, .z = emitter.shape.start.z},
                (Vector3){.x = emitter.shape.start.x + w, .y = emitter.shape.start.y, .z = emitter.shape.start.z + l},
                BLUE);
            DrawLine3D(
                (Vector3){.x = emitter.shape.start.x + w, .y = emitter.shape.start.y, .z = emitter.shape.start.z + l},
                (Vector3){.x = emitter.shape.start.x, .y = emitter.shape.start.y, .z = emitter.shape.start.z + l},
                BLUE);
            DrawLine3D(
                (Vector3){.x = emitter.shape.start.x, .y = emitter.shape.start.y, .z = emitter.shape.start.z + l},
                (Vector3){.x = emitter.shape.start.x, .y = emitter.shape.start.y, .z = emitter.shape.start.z},
                BLUE);

        }
        else if (emitter.shape.type == ST_Sphere)
        {
            DrawCircle3D((Vector3){.x = emitter.shape.start.x,
                                   .y = emitter.shape.start.y,
                                   .z = emitter.shape.start.z},
                         5,
                         (Vector3){.x = 0, .y = 0, .z = 0},
                         0,
                         BLUE);

            DrawCircle3D((Vector3){.x = emitter.shape.start.x,
                                   .y = emitter.shape.start.y,
                                   .z = emitter.shape.start.z},
                         5,
                         (Vector3){.x = 0, .y = 1, .z = 0},
                         90,
                         BLUE);

            DrawCircle3D((Vector3){.x = emitter.shape.start.x,
                                   .y = emitter.shape.start.y,
                                   .z = emitter.shape.start.z},
                         5,
                         (Vector3){.x = 1, .y = 0, .z = 0},
                         90,
                         BLUE);
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
