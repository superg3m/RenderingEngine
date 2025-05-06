#pragma once
#include <types.h>
#include <entity.h>

typedef struct GameState {
    CKG_Arena* permanent_arena;   // General-purpose permanent memory
    CKG_Arena* entity_arena;      // For entity structss
    CKG_Arena* asset_arena;       // Textures, models, sounds, etc.
    CKG_Arena* evictable_arena;  // For things that can be evicted
    CKG_Arena* scratch_arena;     // Frame-local scratch (temporary, reset every tick/frame)

    struct {
        float gravity;    // defaults to -9.8
        float time_scale; // defaults to  1.0
    } rules;

    u64 next_unique_reference_id; // stable and unique
    Entity* entities[256];
    u32 entity_count;
} GameState;

EntityID game_state_generate_entity_id(GameState* game, Entity* e);