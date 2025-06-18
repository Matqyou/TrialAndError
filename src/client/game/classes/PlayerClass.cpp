#include "PlayerClass.h"
#include "./ClassTypes/HumanClass.h"
#include "./ClassTypes/CyborgClass.h"
#include "./ClassTypes/ZombieClass.h"
#include "./ClassTypes/VampireClass.h"

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
