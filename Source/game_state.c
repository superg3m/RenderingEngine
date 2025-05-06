#include <game_state.h>

Entity* game_state_get_entity(GameState* game, EntityID identifer) {
    Entity* entities = (Entity*)game->entity_arena.base_address;
    return entities[identifer.entity_index];
}

Entity* game_state_push_entity(GameState* game) {
    Entity* e = ckg_arena_push(game->entity_arena, Entity);
    e->identifer.entity_index = game->entity_count;
    e->identifer.reference_id = game->next_unique_reference_id++;

    return e;
}

internal GameState deserialize_game_state() {
    GameState game = {0};
    game.rules.gravity = -9.8;
    game.rules.time_scale = 1.0;

    // if there does not exist a serialized file
    // just defaults

    return game;
}

GameState game_state_restore(u8* memory, size_t memory_size_bytes) {
    GameState game = deserialize_game_state(); // Loads saved game state values (e.g., entity_count, rules, etc.)

    // Partition memory manually
    size_t arena_size = memory_size_bytes / 5; // Divide equally among 5 arenas

    game.permanent_arena  = ckg_arena_create_fixed(memory + arena_size * 0, arena_size);
    game.entity_arena     = ckg_arena_create_fixed(memory + arena_size * 1, arena_size);
    game.asset_arena      = ckg_arena_create_fixed(memory + arena_size * 2, arena_size);
    game.evictable_arena  = ckg_arena_create_fixed(memory + arena_size * 3, arena_size);
    game.scratch_arena    = ckg_arena_create_fixed(memory + arena_size * 4, arena_size);

    return game;
}