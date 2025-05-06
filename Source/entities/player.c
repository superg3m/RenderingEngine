#include <game_state.h>

Entity* player_create(GameState* game) {
    // texture to load/retrive here
    Entity* player = NULLPTR; // game_state_push_entity(game->entity_arena); // fills out the EntityID
    player->type = ENTITY_TYPE_PLAYER;
    player->resource.max_health = 100;
    player->resource.health = player->resource.max_health;
    game_state_generate_entity_id(game, player); // (THIS SHOULD ALSO SET THE entity index and reference_id)

    // player.health = 100;
    // player.texture_id

    return player;
}