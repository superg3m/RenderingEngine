#pragma once
#include <types.h>

typedef enum EntityType {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_DRAGON,
    ENTITY_TYPE_PRESSURE_PLATE,
    ENTITY_TYPE_DOOR,
    ENTITY_TYPE_LEVER,
    ENTITY_TYPE_TRAP,
    ENTITY_TYPE_SEQUENCE_CONTROLLER,
} EntityType;

typedef enum StatusEffect {
    STATUS_ON_FIRE     = 0x01,
    STATUS_POISONED    = 0x02,
    STATUS_ROOTED      = 0x04,
    STATUS_INVINCIBLE  = 0x08,
    STATUS_STUNNED     = 0x10,
    STATUS_STARVING    = 0x20,
} StatusEffect;

typedef enum InteractionType {
    INTERACTION_NONE,
    INTERACTION_TOGGLE,     // On/off switch
    INTERACTION_MOMENTARY,  // Active while pressed
    INTERACTION_PERMANENT,  // Stays active once triggered
    INTERACTION_SEQUENCE,   // Must be triggered in correct order
} InteractionType;

typedef enum IteractionState {
    INTERACTION_INACTIVE,
    INTERACTION_ACTIVE,
    INTERACTION_COOLDOWN,
} IteractionState;

typedef enum ActivationRule {
    ACTIVATION_RULE_ANY,       // Any trigger activates
    ACTIVATION_RULE_ALL,       // All triggers must be active
    ACTIVATION_RULE_SEQUENCE,  // Must be activated in a specific sequence
} ActivationRule;

typedef struct {
    u32 fire_damage;
    u32 physical_damage;
    u32 cold_damage;
} Damage;

typedef struct {
    u32 flags;
    u32 duration_in_seconds;
} StatusEffect;

typedef struct {
    u32 max_health;
    u32 health;

    u32 max_stamina;
    u32 stamina;

    u32 max_mana;
    u32 mana;
} Resource;

typedef struct Interactable {
    bool is_interactable;
    InteractionType type;
    IteractionState state;
    float active_timer;
    float cooldown_timer;

    // Sequence logic
    s32 true_sequence_value; // set to 0, 1, 2 and so on
    s32 sequence_order; // default set to -1 as placeholder then overwriten by true_sequence_value when trigger is active.
} Interactable;

typedef struct {
    bool is_triggerable;
    bool active;
    ActivationRule activation_rule;

    Entity** interactables; // for sequence interate through check if first non-negative number is 0 then 0 + 1 and so on
} Triggerable;

typedef struct Entity {
    // base
    u64 unique_reference_id;
    u32 entity_index;
    EntityType type;
    Transform transform;
    Mesh mesh;
    Material material;

    // gameplay
    Damage damage;
    StatusEffect status_effect;
    Resource resource;

    // puzzles
    Interactable interactable;
    Triggerable triggerable;
} Entity;