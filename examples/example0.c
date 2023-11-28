#include <stdio.h>

#include "common.h"



void
link_list_unittest() {

    // move_first_to_empty
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        LinkedList *from = &a;
        LinkedList *to = NULL;

        linked_list_move_item(&from, &to);
        assert(to != NULL);
        assert(to == &a);
        assert(a.prev == NULL);
        assert(a.next == NULL);
        assert(from != NULL);
        assert(from == &b);
        assert(b.prev == NULL);
        assert(b.next == &c);
        assert(c.prev == &b);
        assert(c.next == NULL);
    }
    // move_mid_to_empty
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        LinkedList *from = &b;
        LinkedList *to = NULL;

        linked_list_move_item(&from, &to);
        assert(to != NULL);
        assert(to == &b);
        assert(b.prev == NULL);
        assert(b.next == NULL);
        assert(from != NULL);
        assert(from == &c);
        assert(a.prev == NULL);
        assert(a.next == &c);
        assert(c.prev == &a);
        assert(c.next == NULL);
    }

    // move_last_to_empty
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        LinkedList *from = &c;
        LinkedList *to = NULL;

        linked_list_move_item(&from, &to);
        assert(to != NULL);
        assert(to == &c);
        assert(c.prev == NULL);
        assert(c.next == NULL);
        assert(from == NULL);
        assert(a.prev == NULL);
        assert(a.next == &b);
        assert(b.prev == &a);
        assert(b.next == NULL);
    }


    // move_first_to_first
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        LinkedList d;
        LinkedList e;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        d.prev = NULL;
        d.next = &e;
        e.prev = &d;
        e.next = NULL;

        LinkedList *from = &a;
        LinkedList *to = &d;

        linked_list_move_item(&from, &to);
        assert(from != NULL);
        assert(from == &b);
        assert(b.prev == NULL);
        assert(b.next == &c);
        assert(c.prev == &b);
        assert(c.next == NULL);

        assert(to != NULL);
        assert(to == &a);
        assert(a.prev == NULL);
        assert(a.next == &d);
        assert(d.prev == &a);
        assert(d.next == &e);
        assert(e.prev == &d);
        assert(e.next == NULL);
    }



    // move_mid_to_mid
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        LinkedList d;
        LinkedList e;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        d.prev = NULL;
        d.next = &e;
        e.prev = &d;
        e.next = NULL;

        LinkedList *from = &b;
        LinkedList *to = &e;

        linked_list_move_item(&from, &to);
        assert(from != NULL);
        assert(from == &c);
        assert(a.prev == NULL);
        assert(a.next == &c);
        assert(c.prev == &a);
        assert(c.next == NULL);

        assert(to != NULL);
        assert(to == &b);
        assert(d.prev == NULL);
        assert(d.next == &b);
        assert(b.prev == &d);
        assert(b.next == &e);
        assert(e.prev == &b);
        assert(e.next == NULL);
    }
}



int main(int argc, char *argv[])
{

    link_list_unittest();

    const int screenWidth = 800;
    const int screenHeight = 400;
    InitWindow(screenWidth, screenHeight, "Particle Emitter - casensiom");


    // Define the camera to look into our 3d world
    Camera camera = {0};
    camera.position = (Vector3){0, 0, 1000};       // Camera position
    camera.target = (Vector3){0, 0, 0};         // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};    // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                        // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;     // Camera projection type


    EmitConfiguration configuration;
    configuration.particlesPerSecond = 30;
    configuration.lifespan = (Rangef){.min = 2, .max = 3};
    configuration.speed = (Rangef){.min = 1.2, .max = 1.5};
    configuration.colorRed = (Rangef){.min = 0, .max = 0.1};
    configuration.colorGreen = (Rangef){.min = 0.8, .max = 1};
    configuration.colorBlue = (Rangef){.min = 0, .max = 0.1};
    configuration.colorAlpha = (Rangef){.min = 1, .max = 1};
    configuration.scale = (Rangef){.min = 0.1, .max = 2};
    configuration.rotation = (Rangef){.min = 1, .max = 1};
    Emitter emitter = particle_emitter_create(configuration);
    
    emitter.environment.friction.x = 0.999;
    emitter.environment.friction.y = 0.999;
    emitter.environment.friction.z = 0.999;

    emitter.shape.type = ST_Cube;
    emitter.shape.start = (Vector3d){.x = -10, .y = 10,  .z = 0};
    emitter.shape.end   = (Vector3d){.x = 10,  .y = -10, .z = 0};

    // emitter.environment.forces = PE_CREATE_ARRAY(Force, 4);
    // PE_INSERT(emitter.environment.forces, ((Force) { .pos = (Vector3d){.x =-300, .y =-100, .z = 0}, .force = -0.06 }));
    // PE_INSERT(emitter.environment.forces, ((Force) { .pos = (Vector3d){.x =-300, .y = 100, .z = 0}, .force = -0.06 }));
    // PE_INSERT(emitter.environment.forces, ((Force) { .pos = (Vector3d){.x = 300, .y = 100, .z = 0}, .force = 0.1}));
    // PE_INSERT(emitter.environment.forces, ((Force) { .pos = (Vector3d){.x = 300, .y =-100, .z = 0}, .force = 0.1}));


    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        // UpdateCamera(&camera, CAMERA_THIRD_PERSON);
        float dt = GetFrameTime();
        particle_emitter_update(&emitter, dt);

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(camera);
                
                LinkedList *current = emitter.first;
                while(current != NULL) {

                    Color color = (Color){
                        .r = current->item.color.r.value * 255,
                        .g = current->item.color.g.value * 255,
                        .b = current->item.color.b.value * 255,
                        .a = current->item.color.a.value * 255
                        };

                    DrawCircle3D(
                        (Vector3){ .x = current->item.pos.x, 
                                    .y = current->item.pos.y, 
                                    .z = current->item.pos.z}, 
                        current->item.scale.value * 10, 
                        (Vector3){.x= 0, .y= 0, .z= 0}, 
                        0, color);


                    current = current->next;
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
