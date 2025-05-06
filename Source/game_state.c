#include <entity.h>

typedef struct GameState {
    u8* permanent_memory;
    u8* temperary_memory;

    struct {
        float gravity;    // defaults to -9.8
        float time_scale; // defaults to  1.0
    } rules;

    u64 next_unique_reference_id; // stable and unique
    Entity gameplay_entities[256];
    Entity static_entities[256];
} GameState;

internal GameState deserialize_game_state() {
    GameState game = {0};
    game.rules.gravity = -9.8;
    game.rules.time_scale = 1.0;

    // if there does not exist a serialized file
    // just defaults

    return game;
}

GameState game_state_restore(u8* permanent_memory, u8* temperary_memory) {
    GameState game = deserialize_game_state();

    // Date: May 05, 2025
    // NOTE(Jovanni): I have to overwrite these becuase the serialized data
    // is not longer useful after the virtual address space changes on a new instance of the process
    game.permanent_memory = permanent_memory;
    game.temperary_memory = temperary_memory;

    return game;
}