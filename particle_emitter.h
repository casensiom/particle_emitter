// Particle Emitter library - v1.0 - public domain
// https://github.com/casensiom/particle_emitter
//

#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#define PE_DEFINE_ARRAY(TYPE)          \
    typedef struct TYPE##_struct_array \
    {                                  \
        TYPE *items;                   \
        size_t count;                  \
        size_t capacity;               \
    } TYPE##Array;

#define PE_CREATE_ARRAY(TYPE, CAPACITY)                     \
    (TYPE##Array)                                           \
    {                                                       \
        .items = (TYPE *)malloc((CAPACITY) * sizeof(TYPE)), \
        .count = 0,                                         \
        .capacity = (CAPACITY)                              \
    }

#define PE_INSERT(INSTANCE, ITEM)                        \
    {                                                    \
        if ((INSTANCE).capacity > (INSTANCE).count)      \
        {                                                \
            (INSTANCE).items[(INSTANCE).count] = (ITEM); \
            (INSTANCE).count++;                          \
        }                                                \
    }

#define PE_DESTROY_ARRAY(arr) \
    {                         \
        free(arr.items);      \
        arr.items = NULL;     \
        arr.count = 0;        \
        arr.capacity = 0;     \
    }

#ifndef LINEAR_ALGEBRA
#define LINEAR_ALGEBRA

typedef struct vector3d_struct
{
    float x;
    float y;
    float z;
} Vector3d;

#endif // LINEAR_ALGEBRA

typedef struct range_vector3d_struct
{
    Vector3d max;
    Vector3d min;
} RangeVector3d;

typedef struct range_float_struct
{
    float min;
    float max;
} Rangef;

typedef struct interpolate_struct
{
    float start;
    float end;
    float value;
} Interpolate;

typedef struct interpolate_color_struct
{
    Interpolate r;
    Interpolate g;
    Interpolate b;
    Interpolate a;
} InterpolateColor;

typedef struct force_struct
{
    Vector3d pos;
    float force;
} Force;
PE_DEFINE_ARRAY(Force);

typedef struct environment_structs
{
    Vector3d gravity;
    Vector3d friction;
    ForceArray forces;
    RangeVector3d speedLimit;
} Environment;

typedef struct particle_struct
{
    Vector3d pos;
    Vector3d lastPos;
    Vector3d speed;
    Vector3d size;
    Interpolate scale;
    Interpolate rotation;
    InterpolateColor color;
    float lifetime;
    float timestamp;
    size_t id;
} Particle;

typedef struct linked_list_struct
{
    struct linked_list_struct *prev;
    struct linked_list_struct *next;
    Particle item;
} LinkedList;

enum ShapeType
{
    ST_Point,
    ST_Line,
    ST_Sphere,
    ST_Cube
};

typedef struct shape_struct
{
    enum ShapeType type;
    Vector3d start;
    Vector3d end;
    // ST_Point: only uses start.
    // ST_Line: line defined from start to end.
    // ST_Sphere: sphere with center at, and radius as distance between start and end.
    // ST_Cube: cube defined by a corner at start and the opposite corner at end.
} Shape;

typedef struct emit_configuration_struct
{
    Rangef colorRed;
    Rangef colorGreen;
    Rangef colorBlue;
    Rangef colorAlpha;
    Rangef rotation;
    Rangef scale;
    Rangef speed;
    Rangef lifespan;

    float particlesPerSecond;
} EmitConfiguration;

typedef struct emitter_struct
{
    bool active;
    Shape shape;

    LinkedList *first;
    LinkedList *pool;
    LinkedList *allocated;
    size_t capacity;

    Environment environment;
    EmitConfiguration config;

    float pendingParticles;
    float elapsed;
} Emitter;

#if defined(__cplusplus)
extern "C"
{
#endif

    Emitter particle_emitter_create(EmitConfiguration configuration);

    void particle_emitter_update(Emitter *emitter, float dt);

    void particle_emitter_destroy(Emitter *emitter);

    void linked_list_move_item(LinkedList **from, LinkedList **to);

#if defined(__cplusplus)
}
#endif

#define PARTICLE_EMITTER_IMPLEMENTATION
#ifdef PARTICLE_EMITTER_IMPLEMENTATION

static float particle_emitter_randomize(float min, float max)
{
    if (min > max)
    {
        float tmp = max;
        max = min;
        min = tmp;
    }
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

static float particle_emitter_randomize_range(Rangef range)
{
    return particle_emitter_randomize(range.min, range.max);
}

static Vector3d particle_emitter_normalize(Vector3d v)
{
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    v.x /= length;
    v.y /= length;
    v.z /= length;
    return v;
}

static Vector3d particle_emitter_randomize_vector(Rangef range)
{
    float mag = particle_emitter_randomize(range.min, range.max);
    Vector3d v = {
        .x = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f,
        .y = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f,
        .z = 2.0f * ((float)rand() / (float)RAND_MAX) - 1.0f};

    v = particle_emitter_normalize(v);

    return (Vector3d){.x = v.x * mag, .y = v.y * mag, .z = v.z * mag};
}

static Interpolate particle_emitter_randomize_interpolate(Rangef range)
{
    float start = particle_emitter_randomize(range.min, range.max);
    return (Interpolate){
        .start = start,
        .end = particle_emitter_randomize(range.min, range.max),
        .value = start};
}

static float particle_emitter_interpolatef(float start, float end, float ease)
{
    return (start + (end - start) * ease);
}

static void particle_emitter_interpolate(Interpolate *value, float ease)
{
    value->value = particle_emitter_interpolatef(value->start, value->end, ease);
}

Vector3d particle_emitter_direction(Vector3d pos1, Vector3d pos2)
{
    return (Vector3d){.x = pos1.x - pos2.x, .y = pos1.y - pos2.y, .z = pos1.z - pos2.z};
}

float particle_emitter_distance(Vector3d pos1, Vector3d pos2)
{
    return (pos1.x - pos2.x) * (pos1.x - pos2.x) +
           (pos1.y - pos2.y) * (pos1.y - pos2.y) +
           (pos1.z - pos2.z) * (pos1.z - pos2.z);
}

static size_t count_emitted(Emitter *emitter, float dt)
{
    emitter->pendingParticles += dt * emitter->config.particlesPerSecond;
    size_t emit = (size_t)emitter->pendingParticles;
    emitter->pendingParticles -= emit;
    return emit;
}

void linked_list_move_item(LinkedList **from, LinkedList **to)
{

    if (*from == NULL)
    {
        printf("Cant add particle, pool is empty\n");
        return;
    }

    LinkedList *tmp = *to;
    *to = *from;

    if ((*from)->next != NULL)
    {
        ((*from)->next)->prev = (*from)->prev;
    }
    if ((*from)->prev != NULL)
    {
        ((*from)->prev)->next = (*from)->next;
    }
    *from = (*from)->next;

    if (tmp != NULL)
    {
        (*to)->prev = tmp->prev;
        tmp->prev = *to;
    }
    else
    {
        (*to)->prev = NULL;
    }
    if ((*to)->prev != NULL)
    {
        ((*to)->prev)->next = *to;
    }
    (*to)->next = tmp;
}

static Particle *add_particle(Emitter *emitter)
{
    linked_list_move_item(&(emitter->pool), &(emitter->first));
    return &(emitter->first->item);
}

static void remove_particle(Emitter *emitter, LinkedList **item)
{
    linked_list_move_item(item, &(emitter->pool));
}

static Vector3d particle_emitter_random_pos(Emitter *emitter)
{
    Vector3d ret;
    switch (emitter->shape.type)
    {
    case ST_Point:
        ret = emitter->shape.start;
        break;
    case ST_Line:
    {
        float t = ((float)rand() / (float)RAND_MAX);
        Vector3d dir = particle_emitter_direction(emitter->shape.start, emitter->shape.end);
        ret = (Vector3d){.x = emitter->shape.start.x + dir.x * t,
                            .y = emitter->shape.start.y + dir.y * t,
                            .z = emitter->shape.start.z + dir.z * t};
    }
        break;
    case ST_Sphere:
    {
        float r = particle_emitter_distance(emitter->shape.start, emitter->shape.end);
        ret = particle_emitter_randomize_vector((Rangef){.min = r, .max = r});
    }
        break;
    case ST_Cube:
        ret = (Vector3d){
            .x = particle_emitter_randomize(emitter->shape.start.x, emitter->shape.end.x),
            .y = particle_emitter_randomize(emitter->shape.start.y, emitter->shape.end.y),
            .z = particle_emitter_randomize(emitter->shape.start.z, emitter->shape.end.z)};
        break;
    }

    return ret;
}

static void init_particle(Emitter *emitter, Particle *particle)
{
    static size_t id_counter = 0;

    particle->id = ++id_counter;
    particle->timestamp = emitter->elapsed;
    particle->lifetime = particle_emitter_randomize_range(emitter->config.lifespan);

    // TODO: Improve position
    particle->pos = particle_emitter_random_pos(emitter);
    particle->speed = particle_emitter_randomize_vector(emitter->config.speed);

    particle->rotation = particle_emitter_randomize_interpolate(emitter->config.rotation);
    particle->scale = particle_emitter_randomize_interpolate(emitter->config.scale);
    particle->color.r = particle_emitter_randomize_interpolate(emitter->config.colorRed);
    particle->color.g = particle_emitter_randomize_interpolate(emitter->config.colorGreen);
    particle->color.b = particle_emitter_randomize_interpolate(emitter->config.colorBlue);
    particle->color.a = particle_emitter_randomize_interpolate(emitter->config.colorAlpha);
}

static void update_particle_position(Emitter *emitter, Particle *particle, float dt)
{
    Environment *env = &(emitter->environment);
    particle->speed.x *= env->friction.x;
    particle->speed.y *= env->friction.y;
    particle->speed.z *= env->friction.z;

    for (size_t i = 0; i < env->forces.count; ++i)
    {
        Vector3d dir = particle_emitter_direction(particle->pos, env->forces.items[i].pos);
        float dist = particle_emitter_distance(particle->pos, env->forces.items[i].pos);
        float magnitude = env->forces.items[i].force / dist;
        particle->speed.x += dir.x * magnitude * dt;
        particle->speed.y += dir.y * magnitude * dt;
        particle->speed.z += dir.z * magnitude * dt;
    }

    particle->lastPos.x = particle->pos.x;
    particle->lastPos.y = particle->pos.y;
    particle->lastPos.z = particle->pos.z;

    particle->pos.x += particle->speed.x * dt;
    particle->pos.y += particle->speed.y * dt;
    particle->pos.z += particle->speed.z * dt;
}

static void update_particle_attributes(Emitter *emitter, Particle *particle, float t)
{
    float ease = t * t;
    particle_emitter_interpolate(&(particle->color.r), ease);
    particle_emitter_interpolate(&(particle->color.b), ease);
    particle_emitter_interpolate(&(particle->color.b), ease);
    particle_emitter_interpolate(&(particle->color.a), ease);

    particle_emitter_interpolate(&(particle->rotation), ease);
    particle_emitter_interpolate(&(particle->scale), ease);
}

static void update_particles(Emitter *emitter, float dt)
{
    LinkedList *current = emitter->first;
    while (current != NULL)
    {
        float t = (emitter->elapsed - current->item.timestamp) / current->item.lifetime;
        if (t > 1.0)
        {
            LinkedList *next = current->next;
            remove_particle(emitter, &current);
            current = next;
            continue;
        }

        // Move particle
        update_particle_position(emitter, &(current->item), t);

        // Interpolate values
        update_particle_attributes(emitter, &(current->item), dt);

        current = current->next;
    }
}

Emitter particle_emitter_create(EmitConfiguration configuration)
{
    Emitter emitter;

    size_t maxParticles = (size_t)((configuration.particlesPerSecond * configuration.lifespan.max) + 0.5f);

    emitter.capacity = maxParticles > 0 ? maxParticles : 30;
    emitter.allocated = (LinkedList *)malloc(emitter.capacity * sizeof(LinkedList));
    memset(emitter.allocated, 0, (emitter.capacity * sizeof(LinkedList)));
    for (size_t i = 0; i < emitter.capacity - 1; i++)
    {
        emitter.allocated[i].next = &(emitter.allocated[i + 1]);
        emitter.allocated[i + 1].prev = &(emitter.allocated[i]);
    }
    emitter.allocated[0].prev = NULL;
    emitter.allocated[emitter.capacity - 1].next = NULL;

    emitter.first = NULL;
    emitter.pool = emitter.allocated;

    emitter.active = true;
    emitter.shape = (Shape){
        .type = ST_Point,
        .start = (Vector3d){.x = 0, .y = 0, .z = 0},
        .end = (Vector3d){.x = 0, .y = 0, .z = 0}};
    emitter.environment = (Environment){
        .gravity = (Vector3d){.x = 0, .y = 0, .z = 0},
        .friction = (Vector3d){.x = 1, .y = 1, .z = 1},
        .forces = (ForceArray){
            .items = NULL,
            .count = 0,
            .capacity = 0},
        .speedLimit = (RangeVector3d){.max = (Vector3d){.x = FLT_MAX, .y = FLT_MAX, .z = FLT_MAX}, .min = (Vector3d){.x = FLT_MIN, .y = FLT_MIN, .z = FLT_MIN}}};
    emitter.config = configuration;
    emitter.pendingParticles = 0;
    emitter.elapsed = 0;

    return emitter;
}

void particle_emitter_update(Emitter *emitter, float dt)
{
    emitter->elapsed += dt;
    size_t count = count_emitted(emitter, dt);
    for (size_t i = 0; i < count; i++)
    {
        Particle *particle = add_particle(emitter);
        init_particle(emitter, particle);
    }

    update_particles(emitter, dt);
}

void particle_emitter_destroy(Emitter *emitter)
{
    emitter->capacity = 0;
    free(emitter->allocated);
    emitter->first = NULL;
    emitter->pool = NULL;
    PE_DESTROY_ARRAY(emitter->environment.forces);
}

#endif //  PARTICLE_EMITTER_IMPLEMENTATION

#endif // _PARTICLES_H_