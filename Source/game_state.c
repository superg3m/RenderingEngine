#include <entity.h>

typedef struct GameState {
    u8* permanent_memory;
    u8* temperary_memory;

    Entity gameplay_entities[256];
    Entity static_entities[256];
} GameState;