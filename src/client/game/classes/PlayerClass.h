#pragma once

#include <string>

enum {
	PLAYERCLASS_HUMAN,
	PLAYERCLASS_CYBORG,
	PLAYERCLASS_ZOMBIE,
	PLAYERCLASS_VAMPIRE,
	NUM_PLAYERCLASSES,
};

class Player;
class PlayerClass
{
protected:
	const int class_type;

public:
	PlayerClass(int player_class_type);
	virtual ~PlayerClass() = default;

	// Sense
	[[nodiscard]] const char *GetName() const;

	virtual void ApplyPrimaryClassBonuses(Player *player) = 0;
	virtual void ApplySecondaryClassBonuses(Player *player) = 0;
	virtual void LevelupStats(Player *player) { }

//	static PlayerClass *CreateDefaultClass();
//	static PlayerClass *FromString(const std::string& name);
	static PlayerClass *CreateClass(int player_class_type);

};

inline const char *ClassTypeNames[NUM_PLAYERCLASSES] = {
	"Human",
	"Cyborg",
	"Zombie",
	"Vampire"
};