#include "PlayerClass.h"
#include <client/game/players/classes/ClassTypes/HumanClass.h>
#include <client/game/players/classes/ClassTypes/CyborgClass.h>
#include <client/game/players/classes/ClassTypes/ZombieClass.h>
#include <client/game/players/classes/ClassTypes/VampireClass.h>

#include <algorithm>

PlayerClass *PlayerClass::FromString(const std::string& name)
{
	std::string lower = name;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

	if (lower == "human")
		return new HumanClass();
	else if (lower == "cyborg")
		return new CyborgClass();
	else if (lower == "zombie")
		return new ZombieClass();
	else if (lower == "vampire")
		return new VampireClass();
	return CreateDefaultClass();
}
PlayerClass *PlayerClass::CreateDefaultClass()
{
	return new HumanClass();
}
