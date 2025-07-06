#include "PlayerClass.h"
#include "client/game/classes/ClassTypes/HumanClass.h"
#include "client/game/classes/ClassTypes/CyborgClass.h"
#include "client/game/classes/ClassTypes/ZombieClass.h"
#include "client/game/classes/ClassTypes/VampireClass.h"
#include <algorithm>

PlayerClass::PlayerClass(int player_class_type)
	: class_type(player_class_type)
{

}

const char *PlayerClass::GetName() const
{
	return ClassTypeNames[class_type];
}

PlayerClass *PlayerClass::CreateClass(int player_class_type)
{
	switch (player_class_type)
	{
		case PLAYERCLASS_HUMAN: return new HumanClass();
		case PLAYERCLASS_CYBORG: return new CyborgClass();
		case PLAYERCLASS_ZOMBIE: return new ZombieClass();
		case PLAYERCLASS_VAMPIRE: return new VampireClass();

		case NUM_PLAYERCLASSES:
		default: return new HumanClass();
	}
}
