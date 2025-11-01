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
	ENTITY_ERROR,
	ENTITY_AMMO_BOX,
	ENTITY_ITEM,
	ENTITY_CRATE,
	ENTITY_PROJECTILE,
	ENTITY_CHARACTER,
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
