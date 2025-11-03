//
// Created by T450s on 5/4/2023.
//

#include "Player.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include "client/game/ui/CommonUI.h"
#include "client/game/ui/menus/Menus.h"

#include <SDL3/SDL.h>

Player::Player(const std::string& username, PlayerClass *player_class)
	: character(nullptr),
	  username(),
	  xp(0),
	  level(1),
	  player_class(player_class ? player_class : PlayerClass::CreateClass(PLAYERCLASS_HUMAN))
{
	boss_damage_amplifier = 1;
	base_damage = 10;
	damage_amplifier = 1;
	max_health_amplifier = 1;
	pendingPowerupSelection = false;
	extra_life = false;
	SetUsername(username);
	player_id = GameReference.NextPlayerID();
	nameplate = new TextSurface(CommonUI::fontDefault, username, { 255, 255, 255, 255 });

	wants_gamepad_index = -1;
	gamepad_subscription_id = -1;
	if (true) // player wants to use controller
		wants_gamepad_index = GameReference.GetAvailableGamepadIndex();
	dbg_msg("Player %s looking for controller #%i\n", username.c_str(), wants_gamepad_index);

	GameReference.AddPlayer(this);
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
	unsigned int m_xpForNextLevel = level * 10;
	while (xp >= m_xpForNextLevel)
	{
		xp -= m_xpForNextLevel;
		m_xpForNextLevel = level * 10;
		LevelUp();
	}
}

void Player::LevelUp()
{
	level++;
	if (character)
	{
		// TODO: Change the selectPowerUp to happen every 10 lvls later on
		pendingPowerupSelection = true;
		Menus.levelup_menu->SwitchToThisMenu();
		player_class->LevelupStats(this);
		character->LevelupStats(level);
	}
	base_damage += 1;
}

void Player::AddPowerupUpgrade(Powerup type, int times)
{
	auto type_index = static_cast<size_t>(type);

	pendingPowerupSelection = false;
	upgrade_counts[type_index] += times;

	switch (type)
	{
	case Powerup::ALL_STATS:
		break;
	case Powerup::BOMBS:
		break;
	case Powerup::DOUBLE_DAMAGE:
		IncreaseDamageAmp(2.0);
		break;
	case Powerup::BOSS_DAMAGE:
		IncreaseBossDamageAmp(0.2);
		break;
	case Powerup::EXPLOSIVE_AMMO:
		break;
	case Powerup::EXTRA_LIVES:
		SetExtraLife(true);
		break;
	case Powerup::SPEED:
		break;
	case Powerup::SPIKY:
		break;
	case Powerup::HEALTH:
		IncreaseMaxHealthAmp(1.1);
		break;
	case Powerup::INFINITE_GLOCK_AMMO:
		break;
	default:
		break;
	}
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
	username = FString("%s - %s", player_class->GetName(), new_username.substr(0, MAX_USERNAME_LENGTH).c_str());
	dbg_msg("[Player] &8Setting username to: &f%s\n", username.c_str());
}