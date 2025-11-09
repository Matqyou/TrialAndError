//
// Created by T450s on 5/4/2023.
//

#include "Player.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/ui/CommonUI.h>
#include <shared/string/Strings.h>
#include <SDL3/SDL.h>

Player::Player(const std::string& init_username, PlayerClass *init_player_class)
	: character(nullptr),
	  username(),
	  xp(0),
	  level(1),
	  player_class(init_player_class ? init_player_class : PlayerClass::CreateClass(PLAYERCLASS_HUMAN))
{
	boss_damage_amplifier = 1;
	base_damage = 10;
	damage_amplifier = 1;
	max_health_amplifier = 1;
	extra_life = false;
	username = "Player";
	SetUsername(init_username);
	player_id = GameReference.NextPlayerID();
	nameplate = new TextSurface(CommonUI::fontDefault, init_username, { 255, 255, 255, 255 });

	wants_gamepad_index = -1;
	gamepad_subscription_id = -1;
	if (true) // player wants to use controller
		wants_gamepad_index = GameReference.GetAvailableGamepadIndex();
//	dbg_msg("Player %s looking for controller #%i\n", init_username.c_str(), wants_gamepad_index);

	GameReference.AddPlayer(this);
	dbg_msg("[Player] Added player: &f%s\n", username.c_str());
}

Player::~Player()
{
	delete nameplate;

	if (wants_gamepad_index == 0)
		GamepadsClass::sGamepadOneUpdated.Unsubscribe(gamepad_subscription_id);
	else if (wants_gamepad_index == 1)
		GamepadsClass::sGamepadTwoUpdated.Unsubscribe(gamepad_subscription_id);
	else if (wants_gamepad_index == 2)
		GamepadsClass::sGamepadThreeUpdated.Unsubscribe(gamepad_subscription_id);
	else if (wants_gamepad_index == 3)
		GamepadsClass::sGamepadFourUpdated.Unsubscribe(gamepad_subscription_id);

	wants_gamepad_index = -1;
	gamepad_subscription_id = -1;
}

void Player::GainXP(unsigned int amount)
{
	xp += amount;
	unsigned int m_xpForNextLevel = level * 100;
	while (xp >= m_xpForNextLevel)
	{
		xp -= m_xpForNextLevel;
		m_xpForNextLevel = level * 100;
		LevelUp();
	}
}

void Player::LevelUp()
{
	level++;
	if (character)
	{
//		m_levelUpMenuQueue.push(this->GetLevelUpMenu());
		player_class->LevelupStats(this);
		character->LevelupStats(level);
	}
	base_damage += 1;
}

void Player::AddPowerupUpgrade(Powerup type, int times)
{
	// Cast to size_t because Powerup is an enum class
	auto type_index = static_cast<size_t>(type);
	upgrade_counts[type_index] += times;
}

int Player::GetPowerupUpgradeCount(Powerup type)
{
	// Cast to size_t because Powerup is an enum class
	auto type_index = static_cast<size_t>(type);
	return upgrade_counts[type_index];
}

void Player::SetCharacter(Character *new_character)
{
	if (character && new_character)
		dbg_msg("Player `%s` already has a character.\n", username.c_str());
	else
		character = new_character;

	// no character or doesn't want gamepad, return
	if (!character || wants_gamepad_index == -1)
		return;

	// subscribe to controller adding / removing
	auto gamepad_update_callback = [this](int index, bool added)
	{
		dbg_msg("Gamepad index #%i, added %s\n", index, added ? "true" : "false");
		if (added)
			character->gamepad_index = index;
		else
			character->gamepad_index = -1;
	};

	// todo: cursed, change to array or vector
	if (wants_gamepad_index == 0)
		gamepad_subscription_id = GamepadsClass::sGamepadOneUpdated.Subscribe(gamepad_update_callback);
	else if (wants_gamepad_index == 1)
		gamepad_subscription_id = GamepadsClass::sGamepadTwoUpdated.Subscribe(gamepad_update_callback);
	else if (wants_gamepad_index == 2)
		gamepad_subscription_id = GamepadsClass::sGamepadThreeUpdated.Subscribe(gamepad_update_callback);
	else if (wants_gamepad_index == 3)
		gamepad_subscription_id = GamepadsClass::sGamepadFourUpdated.Subscribe(gamepad_update_callback);
}

void Player::CharacterRemoving()
{
	if (wants_gamepad_index == 0)
		GamepadsClass::sGamepadOneUpdated.Unsubscribe(gamepad_subscription_id);
	else if (wants_gamepad_index == 1)
		GamepadsClass::sGamepadTwoUpdated.Unsubscribe(gamepad_subscription_id);
	else if (wants_gamepad_index == 2)
		GamepadsClass::sGamepadThreeUpdated.Unsubscribe(gamepad_subscription_id);
	else if (wants_gamepad_index == 3)
		GamepadsClass::sGamepadFourUpdated.Unsubscribe(gamepad_subscription_id);

//	wants_gamepad_index = -1; // we still want this controller, it just disconnected
	gamepad_subscription_id = -1;
}

// If username is longer than the limit,
// it will use a substring of it with
// the length of MAX_USERNAME_LENGTH.
void Player::SetUsername(const std::string& new_username)
{
	username = Strings::FString("%s - %s", player_class->GetName(), new_username.substr(0, MAX_USERNAME_LENGTH).c_str());
}