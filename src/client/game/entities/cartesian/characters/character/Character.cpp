//
// Created by 11dpjgailis on 16.03.2023.
//
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/entities/cartesian/characters/CharacterNPC.h>
#include <client/game/entities/item/weapons/EntityGuns.h>
#include <client/game/entities/cartesian/Projectile.h>
#include <client/game/ui/CommonUI.h>

#include <iostream>
#include <vector>
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
	going_direction = Vec2f(0.0f, 0.0f);
	looking_direction = Vec2f(0.0f, 0.0f);
}

// Link textures
LinkTexture Character::sCharacterTexture("entity.character.body");
LinkTexture Character::sTextureBlood("particle.blood");

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
TextSurface *Character::ms_BotNamePlate = nullptr;
const int Character::ms_DefaultControls[NUM_CONTROLS] = { SDL_SCANCODE_W,
														  SDL_SCANCODE_D,
														  SDL_SCANCODE_S,
														  SDL_SCANCODE_A,
														  SDL_SCANCODE_LSHIFT };

Character::Character(GameWorld *world,
					 Player *player,
					 double max_health,
					 const Vec2f& start_pos,
					 const Vec2f& start_vel,
					 bool is_npc)
	: DirectionalEntity(world,
						CHARACTER_ENTITY,
						start_pos,
						Vec2f(35, 35),
						start_vel,
						Vec2f(1.0, 0.0),
						0.93,
						true,
						max_health),
	  m_BaseAcceleration(0.45),
	  m_Hands(this, 10.0),
	  m_Hook(this),
	  m_HealthBar(&m_HealthComponent, 75, 15, 2, 2),
	  input(),
	  m_LastInput(),
	  m_ErrorStatuses(GameReference.GetInterface(), this, !is_npc)
{
	m_Player = player;
	m_ColorHue = m_Player ? 60.0 - double(m_Player->GetIndex() * 30) : 0.0;

	m_BaseDamage = 10;
	m_DamageAmp = 1;
	if (m_Player)
	{
		m_Player->SetCharacter(this);
		m_BaseDamage = m_Player->GetBaseDamage();
		m_DamageAmp = m_Player->GetDamageAmp();
	}

	m_CurrentWeapon = nullptr; // Start by holding nothing
	memset(m_Weapons, 0, sizeof(m_Weapons));

	m_PassiveRegeneration = 0.03; // health per tick when in combat
	m_ActiveRegeneration = 0.1;   // health per tick out of combat
	m_TicksOfCombatUntilRegeneration = (unsigned long long)(10 * Application.GetClock()->GetFramerate()); // todo: implement world tickrate
	m_LastInCombat = 0;

	m_SelectedWeaponIndex = -1;
//	m_GameController = nullptr;
	memset(m_Movement, 0, sizeof(m_Movement));

	m_NPC = is_npc;

	m_AmmoCount = new TextSurface(CommonUI::fontDefault, "0", { 255, 255, 255 });

	auto CoordinateText = FString("Spawned [%ix, %iy]", int(start_pos.x), int(start_pos.y));
	m_CoordinatePlate = new TextSurface(CommonUI::fontDefault, CoordinateText, { 255, 255, 255 });

	m_HealthInt = new TextSurface(CommonUI::fontDefault, "0", { 0, 0, 0 });

	m_HitTicks = 0;
	m_CharacterColor = { 255, 255, 255, 255 };
	m_HookColor = { 255, 255, 255, 255 };
	m_HealthbarColor = { 255, 255, 255, 255 };
	m_HandColor = { 255, 255, 255, 255 };
	m_NameplateColor = { 255, 255, 255, 255 };
	m_HealthRed = { 255, 0, 0, 255 };
	m_HealthBlack = { 0, 0, 0, 255 };

	m_ErrorText = new TextSurface(CommonUI::fontDefault, "m_ErrorText", { 255, 255, 255 });
	// TODO: make vector of weapons instead of array
}

Character::~Character()
{
	delete m_CoordinatePlate;
	delete m_ErrorText;

	auto characters = m_World->GetEntitiesByType(CHARACTER_ENTITY);
	for (Entity *entity : characters)
	{
		Character *Char = (Character *)entity;
		Hook *TargetHook = Char->GetHook();
		if (TargetHook->m_GrabbedEntity == this)
			TargetHook->Unhook();
	}
}

void Character::LevelupStats(unsigned int level)
{
	m_HealthComponent.m_MaxHealth *= m_Player->GetMaxHealthAmp();
	m_HealthComponent.m_MaxHealth += 10 + (1000 - m_HealthComponent.m_MaxHealth) / 10;
	m_HealthComponent.HealFully();
}

void Character::Damage(double damage, Entity *damager)
{
	if (!m_ErrorStatuses.Invincible.IsActive())
	{
		if (m_ErrorStatuses.HealersParadise.IsActive())
		{
			double HealBack = damage;
			if (HealBack > 10) HealBack = 10;
			m_HealthComponent.ChangeHealthBy(+HealBack);
		}

		m_HealthComponent.ChangeHealthBy(-damage);
		m_HitTicks = 7;

		Sound *HurtSound = sHitSounds[rand() % 3].GetSound();
		HurtSound->PlaySound();
	}
	else
	{
		sInvincibleHitSound.GetSound()->PlaySound();
	}

	if (damager != nullptr)
	{
		m_HealthComponent.UpdateDamager(damager);
		m_LastInCombat = m_World->GetTick();
	}
}

void Character::Heal(double value)
{
	m_HealthComponent.ChangeHealthBy(+value);
}

void Character::DropWeapon()
{
	if (!m_CurrentWeapon)
		return;

	WeaponType WepType = m_CurrentWeapon->WepType();
	ItemEntity *NewWeapon;
	switch (WepType)
	{
		case WEAPON_GLOCK:
		{
			NewWeapon = new EntityGlock(m_World, this, (WeaponGlock *)m_Weapons[WEAPON_GLOCK], m_Core.pos);
			m_Weapons[WEAPON_GLOCK] = nullptr;
			break;
		}
		case WEAPON_BURST:
		{
			NewWeapon = new EntityBurst(m_World, this, (WeaponBurst *)m_Weapons[WEAPON_BURST], m_Core.pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
		case WEAPON_SHOTGUN:
		{
			NewWeapon = new EntityShotgun(m_World, this, (WeaponShotgun *)m_Weapons[WEAPON_SHOTGUN], m_Core.pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
		case WEAPON_MINIGUN:
		{
			NewWeapon = new EntityMinigun(m_World, this, (WeaponMinigun *)m_Weapons[WEAPON_MINIGUN], m_Core.pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
		case WEAPON_SNIPER:
		{
			NewWeapon = new EntitySniper(m_World, this, (WeaponSniper *)m_Weapons[WEAPON_SNIPER], m_Core.pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
		case WEAPON_PATERSONNAVY:
		{
			NewWeapon = new EntityPatersonNavy(m_World, this, (PatersonNavy *)m_Weapons[WEAPON_PATERSONNAVY], m_Core.pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
	}

	NewWeapon->Accelerate(m_DirectionalCore.direction * 20);
	NewWeapon->SetRotation(m_DirectionalCore.direction.Atan2());
	NewWeapon->AccelerateRotation(std::fmod(Application.GetRandomizer()->Float(), 0.7f) - 0.35f);
	m_CurrentWeapon = nullptr;
}

void Character::SwitchWeapon(WeaponType type)
{
	// npcs are constantly swapping -_-
	if (!m_Weapons[type] ||
		m_CurrentWeapon == m_Weapons[type])
	{
		m_CurrentWeapon = nullptr;
	}
	else
	{
		sItemSwitchSound.GetSound()->PlaySound();

		m_CurrentWeapon = m_Weapons[type];
		m_AmmoCount->FlagForUpdate();
		m_CurrentWeapon->OnSelected();
	}
}

void Character::RemoveCombat()
{
	auto CurrentTick = m_World->GetTick();
	if (m_TicksOfCombatUntilRegeneration > CurrentTick)
		m_LastInCombat = 0;
	else
		m_LastInCombat = CurrentTick - m_TicksOfCombatUntilRegeneration;
}

void Character::GiveWeapon(ProjectileWeapon *proj_weapon)
{
	WeaponType WepType = proj_weapon->WepType();
	bool HoldingType = m_CurrentWeapon == m_Weapons[WepType];

	if (m_Weapons[WepType])
	{ // Already has this type
		auto Remaining = m_Weapons[WepType]->AddMagAmmo(proj_weapon->GetFullAmmo());
		m_Weapons[WepType]->AddTrueAmmo(Remaining); // The remaining ammo from here gets sent to shadow realm
		delete proj_weapon;

		if (HoldingType)
			m_AmmoCount->FlagForUpdate();
	}
	else
	{
		m_Weapons[WepType] = proj_weapon;
		proj_weapon->SetParent(this);
		m_CurrentWeapon = proj_weapon;
		m_AmmoCount->FlagForUpdate();
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

	if (!m_Weapons[ReloadWeapon])
		return;

	auto AmmoNeeded = m_Weapons[ReloadWeapon]->NeededTrueAmmo();
	auto TakenAmmo = ammo_box->TakeAmmo(AmmoNeeded);
	m_Weapons[ReloadWeapon]->AddTrueAmmo(TakenAmmo);
	if (TakenAmmo > 0)
		sAmmoPickupSound.GetSound()->PlaySound();

	if (m_CurrentWeapon == m_Weapons[ReloadWeapon])
		m_AmmoCount->FlagForUpdate();
}

void Character::EventDeath()
{
	// Play a toned down version particle effect :)
	sTextureBlood.GetTexture()->SetColorMod(255, 0, 0); //
	auto particles = m_World->GetParticles();
	for (int i = 0; i < 50; i++)
	{
		Vec2f vel = {
			m_Core.vel.x * (float)(rand()) / (float)RAND_MAX + 2.0f * ((float)(rand()) / (float)RAND_MAX * 2.0f - 1.0f),
			m_Core.vel.y * (float)(rand()) / (float)RAND_MAX + 2.0f * ((float)(rand()) / (float)RAND_MAX * 2.0f - 1.0f)
		};

		float size = 5.0f + (float)(rand()) / (float)RAND_MAX * 10.0f;
		float orientation = (float)(rand()) / (float)RAND_MAX * 360.0f;
		particles->PlayParticle(
			Particle(
				sTextureBlood.GetTexture(),
				m_Core.pos,
				Vec2f(size, size),
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
		if (!m_Weapons[i])
			continue;

		// In this case the dropper is already dead... so there is no real point to get his address?
		// or maybe there is? No idea man I'm just a bored programmear -_-
		ItemEntity *NewWeapon;
		if (i == WEAPON_GLOCK)
		{
			NewWeapon = new EntityGlock(m_World, this, (WeaponGlock *)m_Weapons[WEAPON_GLOCK], m_Core.pos);
			m_Weapons[WEAPON_GLOCK] = nullptr;
		}
		else if (i == WEAPON_SHOTGUN)
		{
			NewWeapon = new EntityShotgun(m_World, this, (WeaponShotgun *)m_Weapons[WEAPON_SHOTGUN], m_Core.pos);
			m_Weapons[WEAPON_SHOTGUN] = nullptr;
		}
		else if (i == WEAPON_BURST)
		{
			NewWeapon = new EntityBurst(m_World, this, (WeaponBurst *)m_Weapons[WEAPON_BURST], m_Core.pos);
			m_Weapons[WEAPON_BURST] = nullptr;
		}
		else if (i == WEAPON_MINIGUN)
		{
			NewWeapon = new EntityMinigun(m_World, this, (WeaponMinigun *)m_Weapons[WEAPON_MINIGUN], m_Core.pos);
			m_Weapons[WEAPON_MINIGUN] = nullptr;
		}
		else if (i == WEAPON_SNIPER)
		{
			NewWeapon = new EntitySniper(m_World, this, (WeaponSniper *)m_Weapons[WEAPON_SNIPER], m_Core.pos);
			m_Weapons[WEAPON_SNIPER] = nullptr;
		}
		else if (i == WEAPON_PATERSONNAVY)
		{
			NewWeapon = new EntityPatersonNavy(m_World, this, (PatersonNavy *)m_Weapons[WEAPON_PATERSONNAVY], m_Core.pos);
			m_Weapons[WEAPON_PATERSONNAVY] = nullptr;
		}
		else
		{
			throw std::runtime_error("Unhandled weapon drop on death (TODO)");
		}

		NewWeapon->Accelerate(m_DirectionalCore.direction * 5);
		NewWeapon->SetRotation(m_DirectionalCore.direction.Atan2());
		NewWeapon->AccelerateRotation(std::fmod(Application.GetRandomizer()->Float(), 0.35f) - 0.175f);
	}

	if (!m_NPC)
	{ // todo: prob better place for this code
		int NumRealCharacters = 0;
		auto characters = m_World->GetEntitiesByType(CHARACTER_ENTITY);
		for (Entity *entity : characters)
		{
			Character *Char = (Character *)entity;
			if (!Char->IsNPC())
				NumRealCharacters++;
		}
		if (NumRealCharacters == 1)
			m_World->AlliesGone();
	}

	m_Alive = false;
	sDeathSound.GetSound()->PlaySound();
}

void Character::TickKeyboardControls()
{ // TODO: move to characterInput class
	bool Horizontal = m_Movement[CONTROL_LEFT] ^ m_Movement[CONTROL_RIGHT];
	bool Vertical = m_Movement[CONTROL_UP] != m_Movement[CONTROL_DOWN];
	float Unit = Horizontal && Vertical ? M_SQRT1_2 : 1.0f;

//	if (Horizontal || Vertical)
//		m_Input.m_GoingLength = 1.0;

	input.going_direction.x = Horizontal ?
							  m_Movement[CONTROL_LEFT] ? -Unit : Unit
										 : 0.0f;

	input.going_direction.y = Vertical ?
							  m_Movement[CONTROL_UP] ? -Unit : Unit
									   : 0.0f;

	// RequestUpdate look direction
	Vec2f mouse_pos = Application.GetMousePosition();

	Camera& camera = GameReference.GetCamera();
	float zoom = camera.GetZoom();

	Vec2f world_mouse_pos = camera.ScreenToCameraPoint(mouse_pos);
	Vec2f looking_direction = m_Core.pos - world_mouse_pos;
//	auto looking_direction = camera.GetPos() - Vec2f(m_Core.pos) + (mouse_pos - Application.GetHalfResolution()) / zoom;

	input.looking_direction = looking_direction.NormalizeF();
//	m_Input.m_LookingX = drawing->GetCameraX() - m_Core.Pos.x + (mouse_x - m_World->GameWindow()->GetWidth2()) / zoom;
//	m_Input.m_LookingY = drawing->GetCameraY() - m_Core.Pos.y + (mouse_y - m_World->GameWindow()->GetHeight2()) / zoom;
//	m_Input.m_LookingLength = Vec2d(m_Input.m_LookingX, m_Input.m_LookingY).Length();

	auto MouseState = SDL_GetMouseState(nullptr, nullptr);
	input.shooting = MouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT); // If clicked, shoot = true
	input.hooking = MouseState & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
	input.sneaking = m_Movement[CONTROL_SNEAK];

	// Switch weapons TODO mouse input class
	// m_Input.m_NextItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
	//    && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	// m_Input.m_PrevItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT)
	//    && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
}

//void Character::TickGameControllerControls()
//{
//	// Sneaking
//	m_Input.m_Sneaking = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_LEFTSTICK);
//
//	// Check for current joystick values
//	m_GameController->GetJoystick1(m_Input.m_GoingX, m_Input.m_GoingY);
//
//	// AxisX**2 + AxisY**2 <= 1 (keep direction length of 1)
//	m_Input.m_GoingLength = Vec2d(m_Input.m_GoingX, m_Input.m_GoingY).Length();
//	m_Input.m_GoingX /= m_Input.m_GoingLength;
//	m_Input.m_GoingY /= m_Input.m_GoingLength;
//
//	// RequestUpdate look direction
//	double LookingX, LookingY;
//	m_GameController->GetJoystick2(LookingX, LookingY);
//
//	m_Input.m_LookingLength = Vec2d(m_Input.m_LookingX, m_Input.m_LookingY).Length();
//	if (m_Input.m_LookingLength >= 0.6)
//	{
//		m_Input.m_LookingX = LookingX / m_Input.m_LookingLength;
//		m_Input.m_LookingY = LookingY / m_Input.m_LookingLength;
//	}
//
//	// Shooting
//	m_Input.m_Shooting = m_GameController->GetRightTrigger() > 0.7;
//	m_Input.m_Hooking = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
//	m_Input.m_Reloading = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_X);
//
//	// Switch weapons
//	m_Input.m_NextItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
//		&& !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
//	m_Input.m_PrevItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT)
//		&& !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
//	m_Input.m_DeselectItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_UP)
//		&& !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_UP) ||
//		m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN)
//			&& !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
//}

// When in combat heal differently than out of combat
void Character::TickHealth()
{
	auto CurrentTick = m_World->GetTick();
	bool ActiveRegeneration = CurrentTick - m_LastInCombat < m_TicksOfCombatUntilRegeneration;
	m_HealthComponent.ChangeHealthBy(+(ActiveRegeneration ? m_PassiveRegeneration : m_ActiveRegeneration));
	m_HealthComponent.LimitHealthToMax();
}

void Character::TickControls()
{
//	if (m_GameController)
//		TickGameControllerControls();
//	else
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

		float Acceleration = (input.sneaking ? m_BaseAcceleration / 3 : m_BaseAcceleration) *
			(m_ErrorStatuses.Disoriented.IsActive() ? -1.0f : 1.0f) *
			(m_ErrorStatuses.Slowdown.IsActive() ? 0.5f : 1.0f) *
			(m_CurrentWeapon ? 0.9f : 1.0f);
		// Accelerate in that direction
		m_Core.vel += input.going_direction * Acceleration;
	}

	if (input.looking_direction.Length() >= 0.6)
		m_DirectionalCore.direction = input.looking_direction * (m_ErrorStatuses.Disoriented.IsActive() ? -1.0f : 1.0f);
	else if (input.going_direction.Length() >= 0.2)
		m_DirectionalCore.direction = input.going_direction * (m_ErrorStatuses.Disoriented.IsActive() ? -1.0f : 1.0f);

	if (input.next_item ^ input.prev_item
		^ input.deselect_item)
	{ // I hope this works as intended, only 1 at a time | ignore if multiple inputs at the same time
		if (input.deselect_item)
		{
			m_SelectedWeaponIndex = -1;
		}
		else if (input.next_item)
		{
			m_SelectedWeaponIndex++;
			if (m_SelectedWeaponIndex == NUM_WEAPONS)
				m_SelectedWeaponIndex = -1;
		}
		else
		{
			m_SelectedWeaponIndex--;
			if (m_SelectedWeaponIndex == -2)
				m_SelectedWeaponIndex = NUM_WEAPONS - 1;
		}

		if (m_SelectedWeaponIndex == -1)
		{
			m_CurrentWeapon = nullptr;
		}
		else
		{
			SwitchWeapon((WeaponType)m_SelectedWeaponIndex);
		} // yeaaaaaaa
	}
}

void Character::TickHook(double elapsed_seconds)
{
	m_Hook.Tick(elapsed_seconds);
}

void Character::TickCollision()
{
	// Handle collision with other characters
	auto characters = m_World->GetEntitiesByType(CHARACTER_ENTITY);
	for (Entity *entity : characters)
	{
		auto other_character = (Character *)entity;
		if (other_character == this)
			continue;

		EntityCore& other_core = other_character->GetCore();
		Vec2f difference = m_Core.pos - other_core.pos;

		float distance = difference.LengthF();
		if (distance > m_Core.size_ratio + other_core.size_ratio)
			continue;

		if (distance == 0.0f)
		{
			float radians = static_cast<float>(rand() % 360) / 180.0f * static_cast<float>(M_PI);
			difference = FromAngleVec2f(radians); // 1.0f
//			distance_x = cos(radians);
//			distance_y = sin(radians);
//			Distance = 1.0;
		}

		Vec2f push = difference.Normalize() * 0.5f;
//		double XPush = distance_x / Distance * 0.5;
//		double YPush = distance_y / Distance * 0.5;
		m_Core.Accelerate(push);
		other_core.Accelerate(-push);

		if (m_ErrorStatuses.Spiky.IsActive() && m_NPC != other_character->IsNPC())
			other_character->Damage(3, this);
	}

	// Handle collision with crates
	auto crates = m_World->GetEntitiesByType(CRATE_ENTITY);
	for (Entity *entity : crates)
	{
		Crate *crate = (Crate *)entity;
		EntityCore& crate_core = crate->GetCore();
		Vec2f difference = m_Core.pos - crate_core.pos;

		float distance = difference.LengthF();
		float closest_possible_distance = crate_core.size_ratio + m_Core.size_ratio;

		if (distance > closest_possible_distance)
			continue;

		m_Core.pos = crate_core.pos + difference / distance * closest_possible_distance;
	}
}

void Character::TickCurrentWeapon()
{
	if (m_CurrentWeapon)
	{
		auto TempAmmo = m_CurrentWeapon->GetMagAmmo();
		if (input.reloading && !m_LastInput.reloading)
			m_CurrentWeapon->Reload();

		m_CurrentWeapon->Tick();
		auto CurrentAmmo = m_CurrentWeapon->GetMagAmmo();
		if (TempAmmo != CurrentAmmo)
		{
			m_AmmoCount->FlagForUpdate();
			if (!CurrentAmmo && TempAmmo)
			{ m_AmmoCount->SetColor({ 255, 0, 0 }); }
			else
			{ m_AmmoCount->SetColor({ 255, 255, 255 }); }
		}
	}
}

// Function to draw icons for error pickup
void Character::DrawErrorIcons()
{
	m_ErrorStatuses.Draw();
}

void Character::DrawCharacter()
{
	Drawing *drawing = Application.GetDrawing();
	SDL_FRect draw_rect = {
		float(m_Core.pos.x) - float(m_Core.size.x / 2.0),
		float(m_Core.pos.y) - float(m_Core.size.y / 2.0),
		float(m_Core.size.x),
		float(m_Core.size.y)
	};

	sCharacterTexture.GetTexture()
		->SetColorMod(m_CharacterColor.r, m_CharacterColor.g, m_CharacterColor.b);

	Vec2f pointing_direction = m_DirectionalCore.direction * 3.0f + m_Core.vel / 3.0f;
	double pointing_angle = pointing_direction.Atan2() / M_PI * 180.0;
	drawing->RenderTextureRotated(sCharacterTexture.GetTexture()->SDLTexture(),
								  nullptr,
								  draw_rect,
								  pointing_angle - 90.0,
								  nullptr,
								  SDL_FLIP_NONE,
								  GameReference.GetCamera());
}

void Character::DrawHook()
{
	Drawing *drawing = Application.GetDrawing();

	// Draw hook
	if (m_Hook.m_Deployed)
	{
		drawing->SetColor(m_HookColor.r, m_HookColor.g, m_HookColor.b, 255);
		drawing->DrawLine(m_Core.pos, m_Hook.pos, GameReference.GetCamera());
	}
}

void Character::DrawHealthbar()
{
	if (m_NPC)
		return;

	Drawing *drawing = Application.GetDrawing();

	// Render health bar
	if (m_HealthComponent.IsFullHealth())
		return;

	m_HealthBar.SetColor(m_HealthbarColor.r, m_HealthbarColor.g, m_HealthbarColor.b, m_HealthbarColor.a);
	Texture *HealthPlate = m_ErrorStatuses.ConfusingHealth.IsActive() ?
						   m_HealthBar.GetTexture() : m_HealthBar.UpdateTexture();

	float bar_width = HealthPlate->GetWidth() - 20; // Make the health bar slightly smaller
	float bar_height = HealthPlate->GetHeight();
	SDL_FRect healthplate_rect = {
		m_Core.pos.x - bar_width / 2.0f + 10.0f, // Adjust position to the right
		m_Core.pos.y + m_Core.size.y / 2.0f,
		bar_width, bar_height
	};

	if (m_HealthInt->GetFlaggedForUpdate())
	{
		std::string HealthText;
		if (!m_ErrorStatuses.ConfusingHealth.IsActive())
			HealthText = FString("%i/%i", int(m_HealthComponent.m_Health), int(m_HealthComponent.m_MaxHealth));
		else
			HealthText = FString("%i/%i", int(rand() % 999), int(rand() % 999));

		m_HealthInt->SetText(HealthText);
		m_HealthInt->SetColor(m_HealthComponent.GetHealthInPercentage() <= 0.25 ? m_HealthRed : m_HealthBlack);
	}

	Texture *HealthTexture = m_HealthInt->RequestUpdate();
	float HealthTextureW = HealthTexture->GetWidth();
	float HealthTextureH = HealthTexture->GetHeight();
	SDL_FRect HealthIntRect = {
		m_Core.pos.x - HealthTextureW / 4.0f + 10.0f, // Adjust position to the right
		m_Core.pos.y + m_Core.size.y / 2.0f + HealthTextureH / 4.0f,
		HealthTextureW / 2.0f,
		HealthTextureH / 2.0f
	};

	drawing->RenderTexture(HealthPlate->SDLTexture(), nullptr, healthplate_rect, GameReference.GetCamera());
	drawing->RenderTexture(HealthTexture->SDLTexture(), nullptr, HealthIntRect, GameReference.GetCamera());

	// Draw level indicator
	std::string LevelText = FString("LVL %i", m_Player->GetLevel()); // Use the level value directly
	TextSurface LevelSurface(CommonUI::fontSmall, LevelText, { 255, 255, 255 });

	Texture *LevelTexture = LevelSurface.RequestUpdate();
	float LevelTextureW = LevelTexture->GetWidth();
	float LevelTextureH = LevelTexture->GetHeight();
	SDL_FRect LevelRect = {
		m_Core.pos.x - bar_width / 2.0f - LevelTextureW + 5.0f, // Position to the left of the health bar
		m_Core.pos.y + m_Core.size.y / 2.0f + 3.0f,
		LevelTextureW,
		LevelTextureH
	};

	drawing->RenderTexture(LevelTexture->SDLTexture(), nullptr, LevelRect, GameReference.GetCamera());
}

void Character::DrawHands()
{
	Drawing *drawing = Application.GetDrawing();
	auto camera = GameReference.GetCamera();

	m_Hands.SetColor(m_HandColor);
	m_Hands.Draw();

	if (m_CurrentWeapon)
	{
		const Texture *texture = m_CurrentWeapon->GetTexture();
		float Radians = m_DirectionalCore.direction.Atan2F();
		Vec2f HoldPosition = m_CurrentWeapon->GetHoldPosition().RotateF(Radians);

		SDL_FRect WeaponRect = { 0, 0, texture->GetWidth(), texture->GetHeight() };
		WeaponRect.w *= 4.0f;
		WeaponRect.h *= 4.0f;
		WeaponRect.x = m_Core.pos.x + HoldPosition.x;
		WeaponRect.y = m_Core.pos.y + HoldPosition.y - WeaponRect.h / 2.0f;
		SDL_FPoint WeaponPivot = { 0, WeaponRect.h / 2.0f * camera.GetZoom() }; // ??? zoom

		// Laser sight
		if (m_CurrentWeapon->WepType() == WeaponType::WEAPON_SNIPER)
		{
			auto direction = m_DirectionalCore.direction;
			Vec2f current_position = m_Core.pos;
			for (int i = 0; i < 10000; i++)
			{
				current_position += direction;

				// Check against walls
				if (current_position.x < 0 ||
					current_position.y < 0 ||
					current_position.x > m_World->GetWidth() ||
					current_position.y > m_World->GetHeight())
					break;

				// Check against entities
				bool found = false;
				for (Entity *entity : m_World->GetEntities())
				{
					if (entity == this || !entity->HasHealthComponent())
						continue;

					double distance = DistanceVec2f(current_position, entity->GetCore().pos);
					if (distance <= entity->GetCore().size_ratio)
					{
						found = true;
						break;
					}
				}

				if (found) break;
			}
			drawing->SetColor(255, 0, 0, 255);
			drawing->DrawLine(m_Core.pos, current_position, GameReference.GetCamera());
//			drawing->LineCamera(m_Core.pos.x, m_Core.pos.y, current_position.x, current_position.y);
		}

		drawing->RenderTextureRotated(texture->SDLTexture(),
									  nullptr,
									  WeaponRect,
									  Radians / M_PI * 180.0,
									  &WeaponPivot,
									  SDL_FLIP_VERTICAL,
									  GameReference.GetCamera());
//		drawing->RenderTextureExFCamera(texture->SDLTexture(),
//										nullptr,
//										WeaponRect,
//										Radians / M_PI * 180.0,
//										&WeaponPivot,
//										SDL_FLIP_VERTICAL);
	}
}

void Character::DrawNameplate()
{
	double NameVisibility = m_World->GetNamesShown();
	if (NameVisibility == 0.0)
		return;

	int name_opacity = int(NameVisibility * 255.0);

	Drawing *drawing = Application.GetDrawing();

	Texture *NamePlateTexture = m_Player ? m_Player->GetNamePlate()->RequestUpdate() : ms_BotNamePlate->GetTexture();
	float NamePlateW = NamePlateTexture->GetWidth();
	float NamePlateH = NamePlateTexture->GetHeight();
	SDL_FRect NamePlateRect = {
		m_Core.pos.x - NamePlateW / 2.0f,
		m_Core.pos.y - m_Core.size.y / 2.0f - NamePlateH,
		NamePlateW, NamePlateH
	};

	SDL_SetTextureAlphaMod(NamePlateTexture->SDLTexture(), name_opacity);
	drawing->RenderTexture(NamePlateTexture->SDLTexture(), nullptr, NamePlateRect, GameReference.GetCamera());
//	drawing->RenderTextureCamera(NamePlateTexture->SDLTexture(), nullptr, NamePlateRect);

	auto CoordinateText = FString("%ix, %iy", int(m_Core.pos.x), int(m_Core.pos.y));
	m_CoordinatePlate->SetText(CoordinateText);
	m_CoordinatePlate->SetColor(m_NameplateColor);
	Texture *CoordinateTexture = m_CoordinatePlate->RequestUpdate();

	float CoordinatePlateW = NamePlateTexture->GetWidth();
	float CoordinatePlateH = NamePlateTexture->GetHeight();
	SDL_FRect CoordinateRect = {
		m_Core.pos.x - CoordinatePlateW / 2.0f,
		NamePlateRect.y - CoordinatePlateH,
		CoordinatePlateW, CoordinatePlateH
	};
	SDL_SetTextureAlphaMod(CoordinateTexture->SDLTexture(), name_opacity);
	drawing->RenderTexture(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect, GameReference.GetCamera());
//	drawing->RenderTextureCamera(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect);
}

void Character::DrawAmmoCounter()
{
	Drawing *drawing = Application.GetDrawing();
	Camera& camera = GameReference.GetCamera();

	char msg[64];
	std::snprintf(msg, sizeof(msg), "%u/%u", m_CurrentWeapon->GetMagAmmo(), m_CurrentWeapon->GetTrueAmmo());
	m_AmmoCount->SetText(msg);
	if (m_CurrentWeapon->GetMagAmmo() == 0) m_AmmoCount->SetColor({ 255, 0, 0 });
	else m_AmmoCount->SetColor({ 255, 255, 255 });

	Texture *AmmoTexture = m_AmmoCount->RequestUpdate();
	float AmmoTextureW = AmmoTexture->GetWidth();
	float AmmoTextureH = AmmoTexture->GetHeight();
	SDL_FRect AmmoRect = {
		m_Core.pos.x - AmmoTextureW / 2.0f,
		m_Core.pos.y + m_Core.size.y / 2.0f + 20.0f,
		AmmoTextureW, AmmoTextureH
	};
	drawing->RenderTexture(AmmoTexture->SDLTexture(), nullptr, AmmoRect, camera);
//	drawing->RenderTextureCamera(AmmoTexture->SDLTexture(), nullptr, AmmoRect);
}

void Character::DrawErrorName()
{
	if (!m_ErrorStatuses.AnyActive(2.0))
		return;

	Drawing *drawing = Application.GetDrawing();
	auto error_message = FString("Error %s activated", m_ErrorStatuses.GetLastActivated()->Name());

	if (m_ErrorText->GetText() != error_message)
	{
		m_ErrorText->SetText(error_message);
		m_ErrorText->FlagForUpdate();
	}

	Texture *ErrorTexture = m_ErrorText->RequestUpdate();
	float Text_w = ErrorTexture->GetWidth();
	float Text_h = ErrorTexture->GetHeight();
	SDL_FRect ErrorRect = {
		m_Core.pos.x - Text_w / 2.0f,
		m_Core.pos.y - 50.0f,
		Text_w, Text_h
	};
	drawing->RenderTexture(ErrorTexture->SDLTexture(), nullptr, ErrorRect, GameReference.GetCamera());
//	drawing->RenderTextureCamera(ErrorTexture->SDLTexture(), nullptr, ErrorRect);
}

void Character::HandleEvent(const SDL_Event& currentEvent)
{
	if (m_NPC) // || m_GameController)
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
				m_CurrentWeapon = nullptr;
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
			if (currentEvent.key.scancode == ms_DefaultControls[i])
				m_Movement[i] = State;
	}
}

void Character::Tick(double elapsed_seconds)
{
	TickHealth();
	TickControls();      // Parse the inputs of each device keyboard/controller/AI
	TickProcessInputs(); // Do stuff depending on the current held buttons
	TickHook(elapsed_seconds);          // Move hook and or player etc.
	TickCurrentWeapon(); // Shoot accelerate reload etc.
	m_Hands.Tick();
	m_ErrorStatuses.Tick();
//    TickErrorTimers(); // Ticks timer for errors
	// Need every characters to get here..
	// then we apply the accelerations of all
	// hooks and continue with the code below v v v
	// like collisions and velocity tick

	TickCollision();
	TickVelocity(elapsed_seconds); // Move the characters entity
	TickWalls();    // Check if colliding with walls

	// Check if health on screen needs updating
	if ((int)(m_HealthComponent.m_Health) != (int)(m_HealthComponent.m_LastHealth))
		m_HealthInt->FlagForUpdate();

	// Check if coordinate plate on screen needs updating
	if (m_World->GetNamesShown() > 0.05 &&
		((int)(m_Core.pos.x) != (int)(m_LastCore.pos.x) ||
			(int)(m_Core.pos.y) != (int)(m_LastCore.pos.y)))
		m_CoordinatePlate->FlagForUpdate();

	m_HitTicks -= 1;
	if (m_HitTicks < 0)
		m_HitTicks = 0;

	// Summarize the current tick
	m_HealthComponent.TickUpdateLastHealth();
	TickUpdateLastCore();
	memcpy(&m_LastInput, &input, sizeof(CharacterInput));

	if (!m_HealthComponent.IsAlive())
	{
		// Extra life
		if (m_Player && m_Player->GetExtraLifeStatus())
		{
			m_Player->GetCharacter()->GetErrorStatuses().Invincible.Activate();
			return;
		}

		// Die
		EventDeath();
	}
}

void Character::Draw()
{
	double HealthPercentage = m_HealthComponent.GetHealthInPercentage();
	double Hue = m_HitTicks ? 0.0 : m_ColorHue;
	m_CharacterColor = HSLtoRGB({ Hue, 1.0, 0.4 + HealthPercentage * 0.35 });
	m_HookColor = HSLtoRGB({ Hue, 0.5, 1.0 });
	m_HealthbarColor = m_CharacterColor;
	m_HandColor = HSLtoRGB({ Hue, 1.0, 0.2 + HealthPercentage * 0.3 });
	m_NameplateColor = m_HandColor;

	DrawHook();
	DrawHands();
	DrawCharacter();
	DrawHealthbar();
	DrawNameplate();
	DrawErrorIcons();

	if (m_CurrentWeapon)
		DrawAmmoCounter();

	DrawErrorName();
}
