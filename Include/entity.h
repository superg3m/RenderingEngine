#pragma once
#include <types.h>

typedef enum EntityType {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_DRAGON,
} EntityType;

typedef enum StatusEffect {
    STATUS_ON_FIRE = 0x1,
    STATUS_POISONED = 0x2,
    STATUS_ROOTED = 0x4,
    STATUS_INVINCIBLE = 0x8,
    STATUS_STUNNED = 0x10,
    STATUS_STARVING = 0x20,
} StatusEffect;

typedef struct {
    // Base
    u32 id;
    EntityType type;
    float x;
    float y;
    u32 color_texture_id;
    u32 normal_map_id;
    u32 height_map_id;
    // Shader shader;

    struct { // Damage
        u32 fire_damage;
        u32 physical_damage;
        u32 cold_damage;
    };

    struct { // Status Effects
        u32 status_flags;
        u32 duration_in_seconds;
    };

    struct { // Resources
        u32 max_health;
        u32 health;

        u32 max_stamina;
        u32 stamina;

        u32 max_mana;
        u32 mana;
    };
} Entity;