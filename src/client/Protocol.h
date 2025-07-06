//
// Created by Matq on 04/06/2023.
//

#pragma once

#define APPLICATION_VERSION "1.0.0"
#define APPLICATION_NAME "TrialAndError"

enum EntityFormFactor
{
	NORMAL_ENTITY,
	DIRECTIONAL_ENTITY
};

enum EntityType
{
	ERROR_ENTITY,
	AMMO_BOX_ENTITY,
	ITEM_ENTITY,
	CRATE_ENTITY,
	PROJECTILE_ENTITY,
	CHARACTER_ENTITY,
	NUM_ENTITY_TYPES
};

static const char *ENTITY_NAMES[NUM_ENTITY_TYPES] = {
	"Error",
	"AmmoBox",
	"Item",
	"Crate",
	"Projectile",
	"Character"
};

enum class Gamemode
{
	UNDEFINED,
	Solo,
	PvP,
	Sandbox,
	Coop,
	Levels
};
