//
// Created by 11dpjgailis on 16.03.2023.
//
#include <client/game/entities/cartesian/characters/character/Character.h>
#include "client/game/entities/cartesian/item/weapons/EntityGuns.h"
#include <client/game/entities/cartesian/Projectile.h>
#include <client/game/ui/CommonUI.h>

#include <iostream>
#include <cmath>

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440
#endif

CharacterInput::CharacterInput()
{
	shooting = false;
	reloading = false;
	hooking = false;
	sneaking = false;
	next_item = false;
	prev_item = false;
	deselect_item = false;
	going_direction = Vec3f(0, 0, 0);
	looking_direction = Vec3f(0, 0, 0);
}

// Link textures
LoadTexture Character::sCharacterTexture("entity.character.body", AssetsClass::TexturePurpose::GUI_ELEMENT);
LoadTexture Character::sTextureBlood("particle.blood", AssetsClass::TexturePurpose::GUI_ELEMENT);

// Link sounds
LinkSound Character::sHitSounds[3] = {
	LinkSound("entity.character.hurt.1"),
	LinkSound("entity.character.hurt.2"),
	LinkSound("entity.character.hurt.3")
};
LinkSound Character::sInvincibleHitSound("entity.character.hurt.invincible");
LinkSound Character::sDeathSound("entity.character.death");
LinkSound Character::sAmmoPickupSound("entity.ammo_box.pickup.6");
LinkSound Character::sItemSwitchSound("weapon.switch");
LinkSound Character::sThrowItemSound("weapon.throw");
LinkSound Character::sPickupItemSound("weapon.pickup");

// Other
TextSurface *Character::sBotNameplate = nullptr;
const int Character::sDefaultControls[NUM_CONTROLS] = { SDL_SCANCODE_W,
														SDL_SCANCODE_D,
														SDL_SCANCODE_S,
														SDL_SCANCODE_A,
														SDL_SCANCODE_LSHIFT };

Character::Character(Player *our_player,
					 double max_health,
					 const Vec3f& start_pos,
					 const Vec3f& start_vel,
					 bool character_is_npc)
	: DirectionalEntity(ENTITY_CHARACTER,
						start_pos,
						Vec3f(35, 35, 35),
						start_vel,
						Vec3f(1.0, 1.0, 1.0),
						0.93,
						true,
						max_health),
	  base_acceleration(0.45),
	  hands(this, 10.0),
	  hook(this),
	  health_bar(&health_component, 75, 15, 2, 2),
	  input(),
	  last_input(),
	  error_statuses(GameReference.GetInterface(), this, !character_is_npc)
{
	player = our_player;
	color_hue = player ? 60.0 - double(player->GetPlayerID() * 30) : 0.0;
	color_hue = player ? 60.0 - double(player->GetPlayerID() * 30) : 0.0;

	base_damage = 10;
	damage_amplifier = 1.0;

	current_weapon = nullptr; // Start by holding nothing
	memset(weapons, 0, sizeof(weapons));

	passive_regeneration_rate = 0.03; // health per tick when in combat
	active_regeneration_rate = 0.1;   // health per tick out of combat
	ticks_combat_until_regeneration = (unsigned long long)(10 * Application.GetClock()->GetFramerate()); // todo: implement world tickrate
	last_combat_timestamp = 0;
	gamepad_index = -1;

	selected_weapon_index = -1;
//	m_GameController = nullptr;
	memset(movement, 0, sizeof(movement));

	is_npc = character_is_npc;

	ammo_count_plate = new TextSurface(CommonUI::fontDefault, "0", { 255, 255, 255 });

	auto CoordinateText = Strings::FString("Spawned [%ix, %iy]", int(start_pos.x), int(start_pos.y));
	coordinate_plate = new TextSurface(CommonUI::fontDefault, CoordinateText, { 255, 255, 255 });

	health_amount_plate = new TextSurface(CommonUI::fontDefault, "0", { 0, 0, 0 });

	hit_ticks = 0;
	character_color = { 255, 255, 255, 255 };
	hook_color = { 255, 255, 255, 255 };
	healthbar_color = { 255, 255, 255, 255 };
	hand_color = { 255, 255, 255, 255 };
	nameplate_color = { 255, 255, 255, 255 };
	health_red_color = { 255, 0, 0, 255 };
	health_black_color = { 0, 0, 0, 255 };

	error_notification_text = new TextSurface(CommonUI::fontDefault, "m_ErrorText", { 255, 255, 255 });
	// TODO: make vector of weapons instead of array

	if (player)
	{
		player->SetCharacter(this);
		base_damage = player->GetBaseDamage();
		damage_amplifier = player->GetDamageAmp();
	}
}

Character::~Character()
{
	delete coordinate_plate;
	delete error_notification_text;

	for (Entity *other_entity : world->GetEntitiesByType(ENTITY_CHARACTER))
	{
		Character *other_character = (Character *)other_entity;
		Hook *other_hook = other_character->GetHook();
		if (other_hook->grabbed_entity == this)
			other_hook->Unhook();
	}

	if (player)
		player->CharacterRemoving();
}

void Character::LevelupStats(unsigned int level)
{
	health_component.m_MaxHealth *= player->GetMaxHealthAmp();
	health_component.m_MaxHealth += 10 + (1000 - health_component.m_MaxHealth) / 10;
	health_component.HealFully();
}

void Character::Damage(double damage, Entity *damager)
{
	if (!error_statuses.Invincible.IsActive())
	{
		if (error_statuses.HealersParadise.IsActive())
		{
			double HealBack = damage;
			if (HealBack > 10) HealBack = 10;
			health_component.ChangeHealthBy(+HealBack);
		}

		health_component.ChangeHealthBy(-damage);
		hit_ticks = 7;

		Sound *HurtSound = sHitSounds[Application.GetRandomizer()->Int() % 3].GetSound();
		HurtSound->PlaySound();
	}
	else
	{
		sInvincibleHitSound.GetSound()->PlaySound();
	}

	if (damager != nullptr)
	{
		health_component.UpdateDamager(damager);
		last_combat_timestamp = world->GetTick();
	}
}

void Character::Heal(double value)
{
	health_component.ChangeHealthBy(+value);
}

void Character::DropWeapon()
{
	if (!current_weapon)
		return;

	ItemEntity *new_weapon_entity = EntityGuns::CreateItemEntityFromWeaponData(this, current_weapon, core.pos);
	world->AddEntity(new_weapon_entity, true);
	new_weapon_entity->Accelerate(directional_core.direction * 20.0f);
//	new_weapon_entity->SetRotation(directional_core.direction.Atan2()); // todo: 3d
	new_weapon_entity->AccelerateRotation(std::fmod(Application.GetRandomizer()->Double(), 0.7) - 0.35);
	current_weapon = nullptr;
}

void Character::SwitchWeapon(WeaponType type)
{
	// npcs are constantly swapping -_-
	if (!weapons[type] || current_weapon == weapons[type])
	{
		current_weapon = nullptr;
	}
	else
	{
		sItemSwitchSound.GetSound()->PlaySound();

		current_weapon = weapons[type];
		ammo_count_plate->FlagForUpdate();
		current_weapon->OnSelected();
	}
}

void Character::RemoveCombat()
{
	auto CurrentTick = world->GetTick();
	if (ticks_combat_until_regeneration > CurrentTick)
		last_combat_timestamp = 0;
	else
		last_combat_timestamp = CurrentTick - ticks_combat_until_regeneration;
}

void Character::GiveWeapon(ProjectileWeapon *new_weapon)
{
	if (!new_weapon)
		return;

	WeaponType weapon_type = new_weapon->GetWeaponType();
	bool HoldingType = current_weapon == weapons[weapon_type];

	if (weapons[weapon_type])
	{ // Already has this type
		auto Remaining = weapons[weapon_type]->AddMagAmmo(new_weapon->GetFullAmmo());
		weapons[weapon_type]->AddTrueAmmo(Remaining); // The remaining ammo from here gets sent to shadow realm
		delete new_weapon;

		if (HoldingType)
			ammo_count_plate->FlagForUpdate();
	}
	else
	{
		weapons[weapon_type] = new_weapon;
		new_weapon->SetParent(this);
		current_weapon = new_weapon;
		ammo_count_plate->FlagForUpdate();
	}
}

void Character::AmmoPickup(AmmoBox *ammo_box)
{
	WeaponType ReloadWeapon;
	if (ammo_box->Type() == AMMO_GLOCK)
		ReloadWeapon = WEAPON_GLOCK;
	else if (ammo_box->Type() == AMMO_SHOTGUN)
		ReloadWeapon = WEAPON_SHOTGUN;
	else if (ammo_box->Type() == AMMO_BURST)
		ReloadWeapon = WEAPON_BURST;
	else if (ammo_box->Type() == AMMO_MINIGUN)
		ReloadWeapon = WEAPON_MINIGUN;
	else if (ammo_box->Type() == AMMO_SNIPER)
		ReloadWeapon = WEAPON_SNIPER;
	else
		return; // AmmoBox type has no matching gun type

	if (!weapons[ReloadWeapon])
		return;

	auto AmmoNeeded = weapons[ReloadWeapon]->NeededTrueAmmo();
	auto TakenAmmo = ammo_box->TakeAmmo(AmmoNeeded);
	weapons[ReloadWeapon]->AddTrueAmmo(TakenAmmo);
	if (TakenAmmo > 0)
		sAmmoPickupSound.GetSound()->PlaySound();

	if (current_weapon == weapons[ReloadWeapon])
		ammo_count_plate->FlagForUpdate();
}

void Character::EventDeath()
{
	// Play a toned down version particle effect :)
//	sTextureBlood.GetTexture()->SetColorMod(255, 0, 0); //

	Particles *particles = world->GetParticles();
	Randomizer *randomizer = Application.GetRandomizer();
	for (int i = 0; i < 50; i++)
	{
		Vec3f vel = {
			core.vel.x * randomizer->Float() + 2.0f * (randomizer->Float() * 2.0f - 1.0f),
			core.vel.y * randomizer->Float() + 2.0f * (randomizer->Float() * 2.0f - 1.0f),
			core.vel.z * randomizer->Float() + 2.0f * (randomizer->Float() * 2.0f - 1.0f),
		};

		float size = 5.0f + randomizer->Float() * 10.0f;
		float orientation = randomizer->Float() * 360.0f;
		particles->PlayParticle(
			Particle(
				sTextureBlood.GetTexture(),
				core.pos,
				Vec3f(size, size, size),
				vel,
				0.95,
				orientation,
				20,
				0.98,
				200
			)
		);
	}

	for (int i = 0; i < NUM_WEAPONS; i++)
	{
		ProjectileWeapon *weapon_data = weapons[i];
		if (!weapon_data)
			continue;

		// In this case the dropper is already dead... so there is no real point to get his address?
		// or maybe there is? No idea man I'm just a bored programmear -_-
		ItemEntity *new_weapon_entity = EntityGuns::CreateItemEntityFromWeaponData(this, weapon_data, core.pos);
		world->AddEntity(new_weapon_entity, true);
		weapons[i] = nullptr;

		new_weapon_entity->Accelerate(directional_core.direction * 5);
//		new_weapon_entity->SetRotation(directional_core.direction.Atan2()); // todo: 3d
		new_weapon_entity->AccelerateRotation(std::fmod(Application.GetRandomizer()->Float(), 0.35f) - 0.175f);
	}

	if (!is_npc)
	{ // todo: prob better place for this code
		int num_real_characters = 0;
		for (Entity *other_entity : world->GetEntitiesByType(ENTITY_CHARACTER))
		{
			Character *other_character = (Character *)other_entity;
			if (!other_character->IsNPC())
				num_real_characters++;
		}
		if (num_real_characters == 1)
			world->AlliesGone();
	}

	alive = false;
	sDeathSound.GetSound()->PlaySound();
}

void Character::TickKeyboardControls()
{ // TODO: move to characterInput class
	bool Horizontal = movement[CONTROL_LEFT] ^ movement[CONTROL_RIGHT];
	bool Vertical = movement[CONTROL_UP] != movement[CONTROL_DOWN];
	float Unit = Horizontal && Vertical ? M_SQRT1_2 : 1.0f;

//	if (Horizontal || Vertical)
//		m_Input.m_GoingLength = 1.0;

	input.going_direction.x = Horizontal ?
							  movement[CONTROL_LEFT] ? -Unit : Unit
										 : 0.0f;

	input.going_direction.y = Vertical ?
							  movement[CONTROL_UP] ? -Unit : Unit
									   : 0.0f;

	// RequestUpdate look direction
	Vec2f mouse_pos = ApplicationClass::GetMousePosition();

	Camera& camera = GameReference.GetCamera();

	// todo: 3d
//	Vec2f screen_character_pos = camera.CameraToScreenPoint(core.pos);
//	Vec2f looking_direction = mouse_pos - screen_character_pos;
//	input.looking_direction = looking_direction.NormalizeF();

	auto mouse_state = SDL_GetMouseState(nullptr, nullptr);
	input.shooting = mouse_state & SDL_BUTTON_MASK(SDL_BUTTON_LEFT); // If clicked, shoot = true
	input.hooking = mouse_state & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
	input.sneaking = movement[CONTROL_SNEAK];

	// Switch weapons TODO mouse input class
	// m_Input.m_NextItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
	//    && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	// m_Input.m_PrevItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT)
	//    && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
}

void Character::TickGameControllerControls()
{
	Gamepad* gamepad = Gamepads.GetGamepadFromIndex(gamepad_index);

	// Sneaking
	input.sneaking = gamepad->GetButton(SDL_GAMEPAD_BUTTON_LEFT_STICK); // m_GameController->GetButton(SDL_CONTROLLER_BUTTON_LEFTSTICK);

	Vec2f left_joystick = gamepad->GetJoystickLeft();
	if (left_joystick.LengthF() > 1.0f) // todo: check out if there is a better way to calibrate analog stick rather than adding deadzone
		left_joystick = left_joystick.NormalizeF();
//	input.going_direction = left_joystick;
	// todo: 3d

	Vec2f right_joystick = gamepad->GetJoystickRight();
	if (right_joystick.Length() >= 0.6)
	{
		if (right_joystick.LengthF() > 1.0f)
			right_joystick = right_joystick.NormalizeF();
//		input.looking_direction = right_joystick;
	} else {
//		input.looking_direction = left_joystick;
	}

//	Vec2f fixed_right = SquareToCircle(right_joystick);
//	dbg_msg("\n");
//	dbg_msg("Before: %.1fx, %.1fy\n", right_joystick.x, right_joystick.y);
//	dbg_msg("After: %.1fx, %.1fy\n", fixed_right.x, fixed_right.y);

	// Shooting
	input.shooting = gamepad->GetRightTrigger() > 0.1;
	input.hooking = gamepad->GetButton(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
	input.reloading = gamepad->GetButton(SDL_GAMEPAD_BUTTON_WEST);

	// Switch weapons
	input.next_item = gamepad->GetButton(SDL_GAMEPAD_BUTTON_DPAD_RIGHT)
		&& !gamepad->GetLastButton(SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
	input.prev_item = gamepad->GetButton(SDL_GAMEPAD_BUTTON_DPAD_LEFT)
		&& !gamepad->GetLastButton(SDL_GAMEPAD_BUTTON_DPAD_LEFT);
	input.deselect_item = gamepad->GetButton(SDL_GAMEPAD_BUTTON_DPAD_UP)
		&& !gamepad->GetLastButton(SDL_GAMEPAD_BUTTON_DPAD_UP) ||
		gamepad->GetButton(SDL_GAMEPAD_BUTTON_DPAD_DOWN)
			&& !gamepad->GetLastButton(SDL_GAMEPAD_BUTTON_DPAD_DOWN);
}

// When in combat heal differently than out of combat
void Character::TickHealth()
{
	auto CurrentTick = world->GetTick();
	bool ActiveRegeneration = CurrentTick - last_combat_timestamp < ticks_combat_until_regeneration;
	health_component.ChangeHealthBy(+(ActiveRegeneration ? passive_regeneration_rate : active_regeneration_rate));
	health_component.LimitHealthToMax();
}

void Character::TickControls()
{
	if (gamepad_index != -1)
		TickGameControllerControls();
	else
		TickKeyboardControls();

	if (input.looking_direction.Length() <= 0.6 && input.going_direction.Length() > 0.2)
		input.looking_direction = input.going_direction;
}

void Character::TickProcessInputs()
{
	if (input.going_direction.Length() >= 0.2)
	{

		// Fix controller drifting
		// Checks if player is shifting (holding left stick)
		// TODO: bool Shifting = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_LEFTSTICK);

		float Acceleration = (input.sneaking ? base_acceleration / 3 : base_acceleration) *
			(error_statuses.Disoriented.IsActive() ? -1.0f : 1.0f) *
			(error_statuses.Slowdown.IsActive() ? 0.5f : 1.0f) *
			(current_weapon ? 0.9f : 1.0f);
		// Accelerate in that direction
		core.vel += input.going_direction * Acceleration;
	}

	if (input.looking_direction.Length() >= 0.6)
		directional_core.direction = input.looking_direction * (error_statuses.Disoriented.IsActive() ? -1.0f : 1.0f);
	else if (input.going_direction.Length() >= 0.2)
		directional_core.direction = input.going_direction * (error_statuses.Disoriented.IsActive() ? -1.0f : 1.0f);

	if (input.next_item ^ input.prev_item
		^ input.deselect_item)
	{ // I hope this works as intended, only 1 at a time | ignore if multiple inputs at the same time
		if (input.deselect_item)
		{
			selected_weapon_index = -1;
		}
		else if (input.next_item)
		{
			selected_weapon_index++;
			if (selected_weapon_index == NUM_WEAPONS)
				selected_weapon_index = -1;
		}
		else
		{
			selected_weapon_index--;
			if (selected_weapon_index == -2)
				selected_weapon_index = NUM_WEAPONS - 1;
		}

		if (selected_weapon_index == -1)
		{
			current_weapon = nullptr;
		}
		else
		{
			SwitchWeapon((WeaponType)selected_weapon_index);
		} // yeaaaaaaa
	}
}

void Character::TickHook(double elapsed_seconds)
{
	hook.Tick(elapsed_seconds);
}

void Character::TickCollision()
{
	// Handle collision with other characters
	auto characters = world->GetEntitiesByType(ENTITY_CHARACTER);
	for (Entity *entity : characters)
	{
		auto other_character = (Character *)entity;
		if (other_character == this)
			continue;

		EntityCore& other_core = other_character->GetCore();
		Vec3f difference = core.pos - other_core.pos;

		float distance = difference.LengthF();
		if (distance > core.size_ratio + other_core.size_ratio)
			continue;

		if (distance == 0.0f)
		{
			float radians = static_cast<float>(rand() % 360) / 180.0f * static_cast<float>(M_PI);
			// todo: 3d
			// difference = FromAngleVec2f(radians); // 1.0f
		}

//		Vec2f push = difference.Normalize() * 0.5f;
//		core.Accelerate(push);
//		other_core.Accelerate(-push);

		if (error_statuses.Spiky.IsActive() && is_npc != other_character->IsNPC())
			other_character->Damage(3, this);
	}

	// Handle collision with crates
	auto crates = world->GetEntitiesByType(ENTITY_CRATE);
	for (Entity *entity : crates)
	{
		Crate *crate = (Crate *)entity;
		EntityCore& crate_core = crate->GetCore();
		Vec3f difference = core.pos - crate_core.pos;

		float distance = difference.LengthF();
		float closest_possible_distance = crate_core.size_ratio + core.size_ratio;

		if (distance > closest_possible_distance)
			continue;

		core.pos = crate_core.pos + difference / distance * closest_possible_distance;
	}
}

void Character::TickCurrentWeapon()
{
	if (current_weapon)
	{
		auto TempAmmo = current_weapon->GetMagAmmo();
		if (input.reloading && !last_input.reloading)
			current_weapon->Reload();

		current_weapon->Tick();
		auto CurrentAmmo = current_weapon->GetMagAmmo();
		if (TempAmmo != CurrentAmmo)
		{
			ammo_count_plate->FlagForUpdate();
			if (!CurrentAmmo && TempAmmo)
			{ ammo_count_plate->SetColor({ 255, 0, 0 }); }
			else
			{ ammo_count_plate->SetColor({ 255, 255, 255 }); }
		}
	}
}

// Function to draw icons for error pickup
void Character::DrawErrorIcons()
{
	error_statuses.Draw();
}

void Character::DrawCharacter()
{
	SDL_FRect draw_rect = {
		core.pos.x - core.size.x / 2.0f,
		core.pos.y - core.size.y / 2.0f,
		core.size.x,
		core.size.y
	};

//	sCharacterTexture.GetTexture()->SetColorMod(character_color.r, character_color.g, character_color.b);

	Vec3f pointing_direction = directional_core.direction * 3.0f + core.vel / 3.0f;
	// todo: 3d
//	double pointing_angle = pointing_direction.Atan2() / M_PI * 180.0;
//	drawing->RenderTextureRotated(sCharacterTexture.GetTexture()->SDLTexture(),
//								  nullptr,
//								  draw_rect,
//								  pointing_angle - 90.0,
//								  nullptr,
//								  SDL_FLIP_NONE,
//								  GameReference.GetCamera());
}

void Character::DrawHook()
{
//	Drawing *drawing = Application.GetDrawing();
//
//	// Draw hook
//	if (hook.deployed)
//	{
//		drawing->SetColor(hook_color.r, hook_color.g, hook_color.b, 255);
//		// todo: 3d
//		drawing->DrawLine(core.pos, hook.pos, GameReference.GetCamera());
//	}
}

void Character::DrawHealthbar()
{
//	if (is_npc)
//		return;
//
//	DrawingClass *drawing = Application.GetDrawing();
//
//	// Render health bar
//	if (health_component.IsFullHealth())
//		return;
//
//	health_bar.SetColor(healthbar_color.r, healthbar_color.g, healthbar_color.b, healthbar_color.a);
//	Texture *HealthPlate = error_statuses.ConfusingHealth.IsActive() ?
//						   health_bar.GetTexture() : health_bar.UpdateTexture();
//
//	float bar_width = HealthPlate->GetWidth() - 20; // Make the health bar slightly smaller
//	float bar_height = HealthPlate->GetHeight();
//	SDL_FRect healthplate_rect = {
//		core.pos.x - bar_width / 2.0f + 10.0f, // Adjust position to the right
//		core.pos.y + core.size.y / 2.0f,
//		bar_width, bar_height
//	};
//
//	if (health_amount_plate->GetFlaggedForUpdate())
//	{
//		std::string HealthText;
//		if (!error_statuses.ConfusingHealth.IsActive())
//			HealthText = FString("%i/%i", int(health_component.m_Health), int(health_component.m_MaxHealth));
//		else
//			HealthText = FString("%i/%i", int(rand() % 999), int(rand() % 999));
//
//		health_amount_plate->SetText(HealthText);
//		health_amount_plate->SetColor(health_component.GetHealthInPercentage() <= 0.25 ? health_red_color : health_black_color);
//	}
//
//	Texture *HealthTexture = health_amount_plate->RequestUpdate();
//	float HealthTextureW = HealthTexture->GetWidth();
//	float HealthTextureH = HealthTexture->GetHeight();
//	SDL_FRect HealthIntRect = {
//		core.pos.x - HealthTextureW / 4.0f + 10.0f, // Adjust position to the right
//		core.pos.y + core.size.y / 2.0f + HealthTextureH / 4.0f,
//		HealthTextureW / 2.0f,
//		HealthTextureH / 2.0f
//	};
//
//	drawing->RenderTexture(HealthPlate->SDLTexture(), nullptr, healthplate_rect, GameReference.GetCamera());
//	drawing->RenderTexture(HealthTexture->SDLTexture(), nullptr, HealthIntRect, GameReference.GetCamera());
//
//	// Draw level indicator
//	std::string LevelText = FString("LVL %i", player->GetLevel()); // Use the level value directly
//	TextSurface LevelSurface(CommonUI::fontSmall, LevelText, { 255, 255, 255 });
//
//	Texture *LevelTexture = LevelSurface.RequestUpdate();
//	float LevelTextureW = LevelTexture->GetWidth();
//	float LevelTextureH = LevelTexture->GetHeight();
//	SDL_FRect LevelRect = {
//		core.pos.x - bar_width / 2.0f - LevelTextureW + 5.0f, // Position to the left of the health bar
//		core.pos.y + core.size.y / 2.0f + 3.0f,
//		LevelTextureW,
//		LevelTextureH
//	};
//
//	drawing->RenderTexture(LevelTexture->SDLTexture(), nullptr, LevelRect, GameReference.GetCamera());
}

void Character::DrawHands()
{
	// todo: 3d
//	Drawing *drawing = Application.GetDrawing();
//	auto camera = GameReference.GetCamera();
//
//	hands.SetColor(hand_color);
//	hands.Draw();
//
//	// Draw holding gun
//	if (!current_weapon)
//		return;
//
//	Texture *texture = current_weapon->GetTexture();
//	float Radians = directional_core.direction.Atan2F();
//	Vec2f HoldPosition = current_weapon->GetHoldPosition().RotateF(Radians);
//
//	SDL_FRect WeaponRect = { 0, 0, texture->GetWidth(), texture->GetHeight() };
//	WeaponRect.w *= 4.0f;
//	WeaponRect.h *= 4.0f;
//	WeaponRect.x = core.pos.x + HoldPosition.x;
//	WeaponRect.y = core.pos.y + HoldPosition.y - WeaponRect.h / 2.0f;
//	SDL_FPoint WeaponPivot = { 0, WeaponRect.h / 2.0f * camera.GetZoom() }; // ??? zoom
//
//	// Laser sight
//	if (current_weapon->GetWeaponType() == WeaponType::WEAPON_SNIPER)
//	{
//		auto direction = directional_core.direction;
//		Vec2f current_position = core.pos;
//		for (int i = 0; i < 10000; i++)
//		{
//			current_position += direction;
//
//			// Check against walls
//			if (current_position.x < 0 ||
//				current_position.y < 0 ||
//				current_position.x > world->GetWidth() ||
//				current_position.y > world->GetHeight())
//				break;
//
//			// Check against entities
//			bool found = false;
//			for (Entity *entity : world->GetEntities())
//			{
//				if (entity == this || !entity->HasHealthComponent())
//					continue;
//
//				double distance = (current_position - entity->GetCore().pos).LengthF();
//				if (distance <= entity->GetCore().size_ratio)
//				{
//					found = true;
//					break;
//				}
//			}
//
//			if (found) break;
//		}
//		drawing->SetColor(255, 0, 0, 255);
//		drawing->DrawLine(core.pos, current_position, GameReference.GetCamera());
////			drawing->LineCamera(m_Core.pos.x, m_Core.pos.y, current_position.x, current_position.y);
//	}
//
//	drawing->RenderTextureRotated(texture->SDLTexture(),
//								  nullptr,
//								  WeaponRect,
//								  Radians / M_PI * 180.0,
//								  &WeaponPivot,
//								  SDL_FLIP_VERTICAL,
//								  GameReference.GetCamera());
}

void Character::DrawNameplate()
{
//	double NameVisibility = world->GetNamesShown();
//	if (NameVisibility == 0.0)
//		return;
//
//	int name_opacity = int(NameVisibility * 255.0);
//
//	DrawingClass *drawing = Application.GetDrawing();
//
//	Texture *NamePlateTexture = player ? player->GetNamePlate()->RequestUpdate() : sBotNameplate->GetTexture();
//	float NamePlateW = NamePlateTexture->GetWidth();
//	float NamePlateH = NamePlateTexture->GetHeight();
//	SDL_FRect NamePlateRect = {
//		core.pos.x - NamePlateW / 2.0f,
//		core.pos.y - core.size.y / 2.0f - NamePlateH,
//		NamePlateW, NamePlateH
//	};
//
//	SDL_SetTextureAlphaMod(NamePlateTexture->SDLTexture(), name_opacity);
//	drawing->RenderTexture(NamePlateTexture->SDLTexture(), nullptr, NamePlateRect, GameReference.GetCamera());
////	drawing->RenderTextureCamera(NamePlateTexture->SDLTexture(), nullptr, NamePlateRect);
//
//	auto CoordinateText = FString("%ix, %iy", int(core.pos.x), int(core.pos.y));
//	coordinate_plate->SetText(CoordinateText);
//	coordinate_plate->SetColor(nameplate_color);
//	Texture *CoordinateTexture = coordinate_plate->RequestUpdate();
//
//	float CoordinatePlateW = NamePlateTexture->GetWidth();
//	float CoordinatePlateH = NamePlateTexture->GetHeight();
//	SDL_FRect CoordinateRect = {
//		core.pos.x - CoordinatePlateW / 2.0f,
//		NamePlateRect.y - CoordinatePlateH,
//		CoordinatePlateW, CoordinatePlateH
//	};
//	SDL_SetTextureAlphaMod(CoordinateTexture->SDLTexture(), name_opacity);
//	drawing->RenderTexture(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect, GameReference.GetCamera());
////	drawing->RenderTextureCamera(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect);
}

void Character::DrawAmmoCounter()
{
//	DrawingClass *drawing = Application.GetDrawing();
//	Camera& camera = GameReference.GetCamera();
//
//	char msg[64];
//	std::snprintf(msg, sizeof(msg), "%u/%u", current_weapon->GetMagAmmo(), current_weapon->GetTrueAmmo());
//	ammo_count_plate->SetText(msg);
//	if (current_weapon->GetMagAmmo() == 0) ammo_count_plate->SetColor({ 255, 0, 0 });
//	else ammo_count_plate->SetColor({ 255, 255, 255 });
//
//	Texture *AmmoTexture = ammo_count_plate->RequestUpdate();
//	float AmmoTextureW = AmmoTexture->GetWidth();
//	float AmmoTextureH = AmmoTexture->GetHeight();
//	SDL_FRect AmmoRect = {
//		core.pos.x - AmmoTextureW / 2.0f,
//		core.pos.y + core.size.y / 2.0f + 20.0f,
//		AmmoTextureW, AmmoTextureH
//	};
//	drawing->RenderTexture(AmmoTexture->SDLTexture(), nullptr, AmmoRect, camera);
////	drawing->RenderTextureCamera(AmmoTexture->SDLTexture(), nullptr, AmmoRect);
}

void Character::DrawErrorName()
{
//	if (!error_statuses.AnyActive(2.0))
//		return;
//
//	DrawingClass *drawing = Application.GetDrawing();
//	auto error_message = FString("Error %s activated", error_statuses.GetLastActivated()->Name());
//
//	if (error_notification_text->GetText() != error_message)
//	{
//		error_notification_text->SetText(error_message);
//		error_notification_text->FlagForUpdate();
//	}
//
//	Texture *ErrorTexture = error_notification_text->RequestUpdate();
//	float Text_w = ErrorTexture->GetWidth();
//	float Text_h = ErrorTexture->GetHeight();
//	SDL_FRect ErrorRect = {
//		core.pos.x - Text_w / 2.0f,
//		core.pos.y - 50.0f,
//		Text_w, Text_h
//	};
//	drawing->RenderTexture(ErrorTexture->SDLTexture(), nullptr, ErrorRect, GameReference.GetCamera());
////	drawing->RenderTextureCamera(ErrorTexture->SDLTexture(), nullptr, ErrorRect);
}

void Character::HandleEvent(const SDL_Event& currentEvent)
{
	if (is_npc) // || m_GameController)
		return;

	if (currentEvent.type == SDL_EVENT_KEY_DOWN && currentEvent.key.repeat == 0 ||
		currentEvent.type == SDL_EVENT_KEY_UP)
	{ // TODO: Put this into CharacterInput struct
		bool State = currentEvent.type == SDL_EVENT_KEY_DOWN;
		if (State)
		{
			int KeyCode = currentEvent.key.scancode;
			if (KeyCode == SDL_SCANCODE_GRAVE)
			{
				current_weapon = nullptr;
			}
			else if (KeyCode == SDL_SCANCODE_1)
			{
				SwitchWeapon(WEAPON_GLOCK);
			}
			else if (KeyCode == SDL_SCANCODE_2)
			{
				SwitchWeapon(WEAPON_SHOTGUN);
			}
			else if (KeyCode == SDL_SCANCODE_3)
			{
				SwitchWeapon(WEAPON_BURST);
			}
			else if (KeyCode == SDL_SCANCODE_4)
			{
				SwitchWeapon(WEAPON_MINIGUN);
			}
			else if (KeyCode == SDL_SCANCODE_5)
			{
				SwitchWeapon(WEAPON_SNIPER);
			}
			else if (KeyCode == SDL_SCANCODE_6)
			{
				SwitchWeapon(WEAPON_PATERSONNAVY);
			}
			else if (KeyCode == SDL_SCANCODE_Q)
			{
				DropWeapon();
			}
		}

		// Reloads weapon on keyboard player with R button press
		if (currentEvent.key.scancode == SDL_SCANCODE_R)
			input.reloading = State;

		for (int i = 0; i < NUM_CONTROLS; i++)
			if (currentEvent.key.scancode == sDefaultControls[i])
				movement[i] = State;
	}
}

void Character::Tick(double elapsed_seconds)
{
	TickHealth();
	TickControls();      // Parse the inputs of each device keyboard/controller/AI
	TickProcessInputs(); // Do stuff depending on the current held buttons
	TickHook(elapsed_seconds);          // Move hook and or player etc.
	TickCurrentWeapon(); // Shoot accelerate reload etc.
	hands.Tick();
	error_statuses.Tick();
//    TickErrorTimers(); // Ticks timer for errors
	// Need every characters to get here..
	// then we apply the accelerations of all
	// hooks and continue with the code below v v v
	// like collisions and velocity tick

	TickCollision();
	TickVelocity(elapsed_seconds); // Move the characters entity
	TickWalls();    // Check if colliding with walls

	// Check if health on screen needs updating
	if ((int)(health_component.m_Health) != (int)(health_component.m_LastHealth))
		health_amount_plate->FlagForUpdate();

	// Check if coordinate plate on screen needs updating
	if (world->GetNamesShown() > 0.05 &&
		((int)(core.pos.x) != (int)(last_core.pos.x) ||
			(int)(core.pos.y) != (int)(last_core.pos.y)))
		coordinate_plate->FlagForUpdate();

	hit_ticks -= 1;
	if (hit_ticks < 0)
		hit_ticks = 0;

	// Summarize the current tick
	health_component.TickUpdateLastHealth();
	TickUpdateLastCore();
	memcpy(&last_input, &input, sizeof(CharacterInput));

	if (!health_component.IsAlive())
	{
		// Extra life
		if (player && player->GetExtraLifeStatus())
		{
			player->GetCharacter()->GetErrorStatuses().Invincible.Activate();
			return;
		}

		// Die
		EventDeath();
	}
}

void Character::Draw()
{
	double HealthPercentage = health_component.GetHealthInPercentage();
	double Hue = hit_ticks ? 0.0 : color_hue;
	character_color = HSLtoRGB({ Hue, 1.0, 0.4 + HealthPercentage * 0.35 });
	hook_color = HSLtoRGB({ Hue, 0.5, 1.0 });
	healthbar_color = character_color;
	hand_color = HSLtoRGB({ Hue, 1.0, 0.2 + HealthPercentage * 0.3 });
	nameplate_color = hand_color;

	DrawHook();
	DrawHands();
	DrawCharacter();
	DrawHealthbar();
	DrawNameplate();
	DrawErrorIcons();

	if (current_weapon)
		DrawAmmoCounter();

	DrawErrorName();
}
