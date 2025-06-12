//
// Created by Matq on 04/06/2023.
//

#pragma once

enum EntityFormFactor {
    NORMAL_ENTITY,
    DIRECTIONAL_ENTITY
};

enum EntityType {
    ERROR_ENTITY,
    AMMO_BOX_ENTITY,
    ITEM_ENTITY,
    CRATE_ENTITY,
    PROJECTILE_ENTITY,
    CHARACTER_ENTITY,
    NUM_ENTITY_TYPES
};

static const char* ENTITY_NAMES[NUM_ENTITY_TYPES] = {
    "Error",
    "AmmoBox",
    "Item",
    "Crate",
    "Projectile",
    "Character"
};

enum class GameMode
{
    Solo,
    PvP,
    Sandbox,
    Coop,
    Levels
};
