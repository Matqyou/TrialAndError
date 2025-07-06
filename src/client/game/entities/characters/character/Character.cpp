//
// Created by 11dpjgailis on 16.03.2023.
//
#include "Character.h"
#include "../CharacterNPC.h"
#include <cmath>
#include <iostream>
#include "../../item/weapons/EntityGuns.h"
#include "../../Projectile.h"
#include "client/game/ui/CommonUI.h"
#include <vector>

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440
#endif

CharacterInput::CharacterInput()
{
	m_Shooting = false;
	m_Reloading = false;
	m_Hooking = false;
	m_Sneaking = false;
	m_NextItem = false;
	m_PrevItem = false;
	m_DeselectItem = false;
	m_GoingX = 0.0;
	m_GoingY = 0.0;
	m_GoingLength = 0.0;
	m_LookingX = 1.0;
	m_LookingY = 0.0;
	m_LookingLength = 0.0;
}

// Link textures
LinkTexture sCharacterTexture("entity.character.body");
LinkTexture sTextureBlood("particle.blood");

// Link sounds
LinkSound sHitSounds[3] = {
	LinkSound("entity.character.hurt.1"),
	LinkSound("entity.character.hurt.2"),
	LinkSound("entity.character.hurt.3")
};
LinkSound sInvincibleHitSound("entity.character.hurt.invincible");
LinkSound sDeathSound("entity.character.death");
LinkSound sAmmoPickupSound("entity.ammo_box.pickup.6");
LinkSound sItemSwitchSound("weapon.switch");
LinkSound sThrowItemSound("weapon.throw");
LinkSound sPickupItemSound("weapon.pickup");

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
					 const Vec2d& start_pos,
					 const Vec2d& start_vel,
					 bool is_npc)
	: DirectionalEntity(world,
						CHARACTER_ENTITY,
						start_pos,
						Vec2d(35, 35),
						start_vel,
						Vec2d(1.0, 0.0),
						0.93,
						true,
						max_health),
	  m_BaseAcceleration(0.45),
	  m_Hands(this, 10.0),
	  m_Hook(this),
	  m_HealthBar(&m_HealthComponent, 75, 15, 2, 2),
	  m_Input(),
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
	m_TicksOfCombatUntilRegeneration = (unsigned long long)(10 * Application.GetClock()->GetFramerate());
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

	Character *Char = m_World->FirstCharacter();
	for (; Char; Char = (Character *)Char->NextType())
	{
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
			NewWeapon = new EntityGlock(m_World, this, (WeaponGlock *)m_Weapons[WEAPON_GLOCK], m_Core.Pos);
			m_Weapons[WEAPON_GLOCK] = nullptr;
			break;
		}
		case WEAPON_BURST:
		{
			NewWeapon = new EntityBurst(m_World, this, (WeaponBurst *)m_Weapons[WEAPON_BURST], m_Core.Pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
		case WEAPON_SHOTGUN:
		{
			NewWeapon = new EntityShotgun(m_World, this, (WeaponShotgun *)m_Weapons[WEAPON_SHOTGUN], m_Core.Pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
		case WEAPON_MINIGUN:
		{
			NewWeapon = new EntityMinigun(m_World, this, (WeaponMinigun *)m_Weapons[WEAPON_MINIGUN], m_Core.Pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
		case WEAPON_SNIPER:
		{
			NewWeapon = new EntitySniper(m_World, this, (WeaponSniper *)m_Weapons[WEAPON_SNIPER], m_Core.Pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
		case WEAPON_PATERSONNAVY:
		{
			NewWeapon = new EntityPatersonNavy(m_World, this, (PatersonNavy *)m_Weapons[WEAPON_PATERSONNAVY], m_Core.Pos);
			m_Weapons[WepType] = nullptr;
			break;
		}
	}

	NewWeapon->Accelerate(m_DirectionalCore.Direction * 20);
	NewWeapon->SetRotation(m_DirectionalCore.Direction.Atan2());
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
		Vec2d vel = { m_Core.Vel.x * (double)(rand()) / RAND_MAX + 2.0 * ((double)(rand()) / RAND_MAX * 2.0 - 1.0),
					  m_Core.Vel.y * (double)(rand()) / RAND_MAX + 2.0 * ((double)(rand()) / RAND_MAX * 2.0 - 1.0) };

		double size = 5.0 + (double)(rand()) / RAND_MAX * 10.0;
		double orientation = (double)(rand()) / RAND_MAX * 360.0;
		particles->PlayParticle(Particle(sTextureBlood.GetTexture(), m_Core.Pos, Vec2d(size, size), vel, 0.95, orientation, 20, 0.98, 200));
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
			NewWeapon = new EntityGlock(m_World, this, (WeaponGlock *)m_Weapons[WEAPON_GLOCK], m_Core.Pos);
			m_Weapons[WEAPON_GLOCK] = nullptr;
		}
		else if (i == WEAPON_SHOTGUN)
		{
			NewWeapon = new EntityShotgun(m_World, this, (WeaponShotgun *)m_Weapons[WEAPON_SHOTGUN], m_Core.Pos);
			m_Weapons[WEAPON_SHOTGUN] = nullptr;
		}
		else if (i == WEAPON_BURST)
		{
			NewWeapon = new EntityBurst(m_World, this, (WeaponBurst *)m_Weapons[WEAPON_BURST], m_Core.Pos);
			m_Weapons[WEAPON_BURST] = nullptr;
		}
		else if (i == WEAPON_MINIGUN)
		{
			NewWeapon = new EntityMinigun(m_World, this, (WeaponMinigun *)m_Weapons[WEAPON_MINIGUN], m_Core.Pos);
			m_Weapons[WEAPON_MINIGUN] = nullptr;
		}
		else if (i == WEAPON_SNIPER)
		{
			NewWeapon = new EntitySniper(m_World, this, (WeaponSniper *)m_Weapons[WEAPON_SNIPER], m_Core.Pos);
			m_Weapons[WEAPON_SNIPER] = nullptr;
		}
		else if (i == WEAPON_PATERSONNAVY)
		{
			NewWeapon = new EntityPatersonNavy(m_World, this, (PatersonNavy *)m_Weapons[WEAPON_PATERSONNAVY], m_Core.Pos);
			m_Weapons[WEAPON_PATERSONNAVY] = nullptr;
		}
		else
		{
			throw std::runtime_error("Unhandled weapon drop on death (TODO)");
		}

		NewWeapon->Accelerate(m_DirectionalCore.Direction * 5);
		NewWeapon->SetRotation(m_DirectionalCore.Direction.Atan2());
		NewWeapon->AccelerateRotation(std::fmod(Application.GetRandomizer()->Float(), 0.35f) - 0.175f);
	}

	if (!m_NPC)
	{ // prob better place for this code
		int NumRealCharacters = 0;
		for (auto Char = m_World->FirstCharacter(); Char; Char = (Character *)Char->NextType())
		{
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
	double Unit = Horizontal && Vertical ? M_SQRT1_2 : 1.0;

	if (Horizontal || Vertical)
		m_Input.m_GoingLength = 1.0;

	if (Horizontal)
	{
		if (m_Movement[CONTROL_LEFT])
			m_Input.m_GoingX = -Unit;
		else
			m_Input.m_GoingX = Unit;
	}
	else
	{
		m_Input.m_GoingX = 0.0;
	}

	if (Vertical)
	{
		if (m_Movement[CONTROL_UP])
			m_Input.m_GoingY = -Unit;
		else
			m_Input.m_GoingY = Unit;
	}
	else
	{
		m_Input.m_GoingY = 0.0;
	}

	// RequestUpdate look direction
	float XMouse, YMouse;
	SDL_GetMouseState(&XMouse, &YMouse);

	auto camera = GameReference.GetCamera();
	m_Input.m_LookingX = camera.GetPos().x - m_Core.Pos.x + (XMouse - Application.GetWidth2()) / camera.GetZoom();
	m_Input.m_LookingY = camera.GetPos().y - m_Core.Pos.y + (YMouse - Application.GetHeight2()) / camera.GetZoom();
	m_Input.m_LookingLength = Vec2d(m_Input.m_LookingX, m_Input.m_LookingY).Length();

	if (m_Input.m_LookingLength != 0.0)
	{
		m_Input.m_LookingX /= m_Input.m_LookingLength;
		m_Input.m_LookingY /= m_Input.m_LookingLength;
	}
	else
	{
		m_Input.m_LookingX = 1.0;
		m_Input.m_LookingY = 0.0;
	}

	auto MouseState = SDL_GetMouseState(nullptr, nullptr);
	m_Input.m_Shooting = MouseState & SDL_BUTTON_LMASK; // If clicked, shoot = true
	m_Input.m_Hooking = MouseState & SDL_BUTTON_RMASK;
	m_Input.m_Sneaking = m_Movement[CONTROL_SHIFT];

	// Switch weapons TODO mouse input class
	// m_Input.m_NextItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
	//    && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	// m_Input.m_PrevItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT)
	//    && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
}

// Todo: update to SDL3
void Character::TickGameControllerControls()
{
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
}

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
	// Todo: update to SDL3
//	if (m_GameController)
//		TickGameControllerControls();
//	else
	TickKeyboardControls();

	if (m_Input.m_LookingLength <= 0.6 && m_Input.m_GoingLength > 0.2)
	{
		m_Input.m_LookingX = m_Input.m_GoingX;
		m_Input.m_LookingY = m_Input.m_GoingY;
	}
}

void Character::TickProcessInputs()
{
	if (m_Input.m_GoingLength >= 0.2)
	{ // Fix controller drifting
		// Checks if player is shifting (holding left stick)
		// TODO: bool Shifting = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_LEFTSTICK);

		double Acceleration = (m_Input.m_Sneaking ? m_BaseAcceleration / 3 : m_BaseAcceleration) *
			(m_ErrorStatuses.Disoriented.IsActive() ? -1 : 1) *
			(m_ErrorStatuses.Slowdown.IsActive() ? 0.5 : 1) *
			(m_CurrentWeapon ? 0.9 : 1.0);

		// Accelerate in that direction
		m_Core.Vel += Vec2d(m_Input.m_GoingX, m_Input.m_GoingY) * Acceleration;
	}

	if (m_Input.m_LookingLength >= 0.6)
	{
		m_DirectionalCore.Direction = Vec2d(m_Input.m_LookingX, m_Input.m_LookingY) * (m_ErrorStatuses.Disoriented.IsActive()
																					   ? -1 : 1);
	}
	else if (m_Input.m_GoingLength >= 0.2)
	{
		m_DirectionalCore.Direction = Vec2d(m_Input.m_GoingX, m_Input.m_GoingY) * (m_ErrorStatuses.Disoriented.IsActive()
																				   ? -1 : 1);
	}

	if (m_Input.m_NextItem ^ m_Input.m_PrevItem
		^ m_Input.m_DeselectItem)
	{ // I hope this works as intended, only 1 at a time | ignore if multiple inputs at the same time
		if (m_Input.m_DeselectItem)
		{
			m_SelectedWeaponIndex = -1;
		}
		else if (m_Input.m_NextItem)
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

void Character::TickHook()
{
	m_Hook.Tick();
}

void Character::TickCollision()
{
	// Handle collision with other characters
	auto Char = m_World->FirstCharacter();
	for (; Char; Char = (Character *)(Char->NextType()))
	{
		if (Char == this)
			continue;

		EntityCore& EntCore = Char->GetCore();
		double XDistance = m_Core.Pos.x - EntCore.Pos.x;
		double YDistance = m_Core.Pos.y - EntCore.Pos.y;
		double Distance = DistanceVec2d(m_Core.Pos, EntCore.Pos);

		if (Distance > m_Core.sizeRatio + EntCore.sizeRatio)
			continue;
		else if (Distance == 0.0)
		{
			double Radians = (rand() % 360) / 180.0 * M_PI;
			XDistance = cos(Radians);
			YDistance = sin(Radians);
			Distance = 1.0;
		}

		double XPush = XDistance / Distance * 0.5;
		double YPush = YDistance / Distance * 0.5;
		m_Core.Accelerate(XPush, YPush);
		EntCore.Accelerate(-XPush, -YPush);

		if (m_ErrorStatuses.Spiky.IsActive() && m_NPC != Char->IsNPC())
			Char->Damage(3, this);
	}

	// Handle collision with crates
	auto CrateEntity = m_World->FirstCrate();
	for (; CrateEntity; CrateEntity = (Crate *)(CrateEntity->NextType()))
	{
		EntityCore& CrateCore = CrateEntity->GetCore();
		Vec2d Difference = m_Core.Pos - CrateCore.Pos;
		double Distance = Difference.Length();
		double ClosestPossibleDistance = CrateCore.sizeRatio + m_Core.sizeRatio;

		if (Distance > ClosestPossibleDistance)
			continue;

		m_Core.Pos = CrateCore.Pos + Difference / Distance * ClosestPossibleDistance;
	}
}

void Character::TickCurrentWeapon()
{
	if (m_CurrentWeapon)
	{
		auto TempAmmo = m_CurrentWeapon->GetMagAmmo();
		if (m_Input.m_Reloading && !m_LastInput.m_Reloading)
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
	auto drawing = Application.GetDrawing();
	SDL_FRect DrawRect = { float(m_Core.Pos.x) - float(m_Core.Size.x / 2.0),
						   float(m_Core.Pos.y) - float(m_Core.Size.y / 2.0),
						   float(m_Core.Size.x),
						   float(m_Core.Size.y) };

	sCharacterTexture.GetTexture()->SetColorMod(m_CharacterColor.r, m_CharacterColor.g, m_CharacterColor.b);

	Vec2d PointingVector = m_DirectionalCore.Direction * 3.0 + m_Core.Vel / 3.0;
	double PointingAngle = PointingVector.Atan2() / M_PI * 180.0;
	SDL_FlipMode flip = SDL_FLIP_NONE;
	drawing->RenderTextureRotated(sCharacterTexture.GetTexture()->SDLTexture(),
								  nullptr, DrawRect,
								  PointingAngle - 90.0, nullptr,
								  flip, GameReference.GetCamera());
}

void Character::DrawHook()
{
	auto drawing = Application.GetDrawing();

	// Draw hook
	if (m_Hook.m_Deployed)
	{
		drawing->SetColor(m_HookColor.r, m_HookColor.g, m_HookColor.b, 255);
		drawing->DrawLine(Vec2f(m_Core.Pos), Vec2f(m_Hook.m_x, m_Hook.m_y), GameReference.GetCamera());
	}
}

void Character::DrawHealthbar()
{
	if (m_NPC)
		return;

	auto drawing = Application.GetDrawing();

	// Render health bar
	if (m_HealthComponent.IsFullHealth())
		return;

	m_HealthBar.SetColor(m_HealthbarColor.r, m_HealthbarColor.g, m_HealthbarColor.b, m_HealthbarColor.a);
	Texture *HealthPlate = m_ErrorStatuses.ConfusingHealth.IsActive() ? m_HealthBar.GetTexture()
																	  : m_HealthBar.UpdateTexture();

	float HealthBarW = HealthPlate->GetWidth() - 20; // Make the health bar slightly smaller
	float HealthBarH = HealthPlate->GetHeight();
	SDL_FRect HealthplateRect = { float(m_Core.Pos.x - HealthBarW / 2.0) + 10, // Adjust position to the right
								  float(m_Core.Pos.y + m_Core.Size.y / 2.0),
								  HealthBarW, HealthBarH };

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
	double HealthTextureW = HealthTexture->GetWidth();
	double HealthTextureH = HealthTexture->GetHeight();
	SDL_FRect HealthIntRect = { float(m_Core.Pos.x - HealthTextureW / 4.0) + 10, // Adjust position to the right
								float(m_Core.Pos.y + m_Core.Size.y / 2.0 + HealthTextureH / 4.0),
								float(HealthTextureW / 2.0),
								float(HealthTextureH / 2.0) };

	drawing->RenderTexture(HealthPlate->SDLTexture(), nullptr, HealthplateRect, GameReference.GetCamera());
	drawing->RenderTexture(HealthTexture->SDLTexture(), nullptr, HealthIntRect, GameReference.GetCamera());

	// Draw level indicator
	std::string LevelText = FString("LVL %i", m_Player->GetLevel()); // Use the level value directly
	TextSurface LevelSurface(CommonUI::fontSmall, LevelText, { 255, 255, 255 });
	Texture *LevelTexture = LevelSurface.RequestUpdate();
	float LevelTextureW = LevelTexture->GetWidth();
	float LevelTextureH = LevelTexture->GetHeight();
	SDL_FRect LevelRect = {
		float(m_Core.Pos.x - HealthBarW / 2.0) - LevelTextureW + 5, // Position to the left of the health bar
		float(m_Core.Pos.y + m_Core.Size.y / 2.0) + 3,
		LevelTextureW,
		LevelTextureH
	};

	drawing->RenderTexture(LevelTexture->SDLTexture(), nullptr, LevelRect, GameReference.GetCamera());
}

void Character::DrawHands()
{
	auto drawing = Application.GetDrawing();
	auto camera = GameReference.GetCamera();

	m_Hands.SetColor(m_HandColor);
	m_Hands.Draw();

	if (m_CurrentWeapon)
	{
		const Texture *texture = m_CurrentWeapon->GetTexture();
		double Radians = m_DirectionalCore.Direction.Atan2();
		Vec2d HoldPosition = m_CurrentWeapon->GetHoldPosition();
		HoldPosition.Rotate(Radians);

		SDL_FRect WeaponRect = { 0, 0, (float)texture->GetWidth(), (float)texture->GetHeight() };
		WeaponRect.w *= 4;
		WeaponRect.h *= 4;
		WeaponRect.x = float(m_Core.Pos.x + HoldPosition.x);
		WeaponRect.y = float(m_Core.Pos.y + HoldPosition.y - float(WeaponRect.h) / 2.0);
		SDL_FPoint WeaponPivot = { 0, float(double(WeaponRect.h) / 2.0 * camera.GetZoom()) };

		// Laser sight
		if (m_CurrentWeapon->WepType() == WeaponType::WEAPON_SNIPER)
		{
			auto direction = m_DirectionalCore.Direction;
			Vec2d current_position = m_Core.Pos;
			for (int i = 0; i < 10000; i++)
			{
				current_position += direction;

				// Check against walls
				if (current_position.x < 0 ||
					current_position.y < 0 ||
					current_position.x > m_World->GetWidth() ||
					current_position.y > m_World->GetHeight())
				{
					break;
				}

				// Check against entities
				bool found = false;
				auto entity = m_World->FirstEntity();
				for (; entity != nullptr; entity = entity->Next())
				{
					if (entity == this || !entity->HasHealthComponent())
						continue;

					double distance = DistanceVec2d(current_position, entity->GetCore().Pos);
					if (distance <= entity->GetCore().sizeRatio)
					{
						found = true;
						break;
					}
				}

				if (found) break;
			}
			drawing->SetColor(255, 0, 0, 255);
			drawing->DrawLine(Vec2f(m_Core.Pos), Vec2f(current_position), camera);
		}

		drawing->RenderTextureRotated(texture->SDLTexture(),
									  nullptr, WeaponRect,
									  Radians / M_PI * 180.0, &WeaponPivot,
									  SDL_FLIP_VERTICAL, camera);
	}
}

void Character::DrawNameplate()
{
	double NameVisibility = m_World->GetNamesShown();
	if (NameVisibility == 0.0)
		return;

	auto drawing = Application.GetDrawing();

	int Opacity = int(NameVisibility * 255.0);
	Texture *NamePlateTexture = m_Player ? m_Player->GetNamePlate()->RequestUpdate() : ms_BotNamePlate->GetTexture();

	float NamePlateW = NamePlateTexture->GetWidth();
	float NamePlateH = NamePlateTexture->GetHeight();
	SDL_FRect NamePlateRect = { float(m_Core.Pos.x - NamePlateW / 2.0),
								float(m_Core.Pos.y - m_Core.Size.y / 2.0 - NamePlateH),
								NamePlateW, NamePlateH };

	SDL_SetTextureAlphaMod(NamePlateTexture->SDLTexture(), Opacity);
	drawing->RenderTexture(NamePlateTexture->SDLTexture(), nullptr, NamePlateRect, GameReference.GetCamera());

	auto CoordinateText = FString("%ix, %iy", int(m_Core.Pos.x), int(m_Core.Pos.y));
	m_CoordinatePlate->SetText(CoordinateText);
	m_CoordinatePlate->SetColor(m_NameplateColor);
	Texture *CoordinateTexture = m_CoordinatePlate->RequestUpdate();

	float CoordinatePlateW = NamePlateTexture->GetWidth();
	float CoordinatePlateH = NamePlateTexture->GetHeight();
	SDL_FRect CoordinateRect = { float(m_Core.Pos.x - CoordinatePlateW / 2.0),
								 float(NamePlateRect.y - CoordinatePlateH),
								 CoordinatePlateW, CoordinatePlateH };
	SDL_SetTextureAlphaMod(CoordinateTexture->SDLTexture(), Opacity);
	drawing->RenderTexture(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect, GameReference.GetCamera());
}

void Character::DrawAmmoCounter()
{
	auto drawing = Application.GetDrawing();

	char msg[64];
	std::snprintf(msg, sizeof(msg), "%u/%u", m_CurrentWeapon->GetMagAmmo(), m_CurrentWeapon->GetTrueAmmo());
	m_AmmoCount->SetText(msg);
	if (m_CurrentWeapon->GetMagAmmo() == 0) m_AmmoCount->SetColor({ 255, 0, 0 });
	else m_AmmoCount->SetColor({ 255, 255, 255 });

	Texture *AmmoTexture = m_AmmoCount->RequestUpdate();
	float AmmoTextureW = AmmoTexture->GetWidth();
	float AmmoTextureH = AmmoTexture->GetHeight();
	SDL_FRect AmmoRect = { float(m_Core.Pos.x - AmmoTextureW / 2.0),
						   float(m_Core.Pos.y + m_Core.Size.y / 2 + 20),
						   AmmoTextureW, AmmoTextureH };
	drawing->RenderTexture(AmmoTexture->SDLTexture(), nullptr, AmmoRect, GameReference.GetCamera());
}

void Character::DrawErrorName()
{
	if (!m_ErrorStatuses.AnyActive(2.0))
		return;

	auto drawing = Application.GetDrawing();
	auto error_message = FString("Error %s activated", m_ErrorStatuses.GetLastActivated()->Name());
	if (m_ErrorText->GetText() != error_message)
	{
		m_ErrorText->SetText(error_message);
		m_ErrorText->FlagForUpdate();
	}

	Texture *ErrorTexture = m_ErrorText->RequestUpdate();
	float Text_w = ErrorTexture->GetWidth();
	float Text_h = ErrorTexture->GetHeight();
	SDL_FRect ErrorRect = { float(m_Core.Pos.x - (double)Text_w / 2.0),
							float(m_Core.Pos.y - 50.0),
							Text_w, Text_h };
	drawing->RenderTexture(ErrorTexture->SDLTexture(), nullptr, ErrorRect, GameReference.GetCamera());
}

void Character::Event(const SDL_Event& currentEvent)
{
	if (m_NPC)
		return;

//	if (m_GameController) // Todo: update sdl3
//		return;

	if (currentEvent.type == SDL_EVENT_KEY_DOWN && currentEvent.key.repeat == 0 ||
		currentEvent.type == SDL_EVENT_KEY_UP)
	{ // TODO: Put this into CharacterInput struct
		bool State = currentEvent.type == SDL_EVENT_KEY_DOWN;
		if (State)
		{
			int KeyCode = currentEvent.key.scancode;
			if (KeyCode == SDL_SCANCODE_GRAVE)
				m_CurrentWeapon = nullptr;
			else if (KeyCode == SDL_SCANCODE_1)
				SwitchWeapon(WEAPON_GLOCK);
			else if (KeyCode == SDL_SCANCODE_2)
				SwitchWeapon(WEAPON_SHOTGUN);
			else if (KeyCode == SDL_SCANCODE_3)
				SwitchWeapon(WEAPON_BURST);
			else if (KeyCode == SDL_SCANCODE_4)
				SwitchWeapon(WEAPON_MINIGUN);
			else if (KeyCode == SDL_SCANCODE_5)
				SwitchWeapon(WEAPON_SNIPER);
			else if (KeyCode == SDL_SCANCODE_6)
				SwitchWeapon(WEAPON_PATERSONNAVY);
			else if (KeyCode == SDL_SCANCODE_Q)
				DropWeapon();
		}

		// Reloads weapon on keyboard player with R button press
		if (currentEvent.key.scancode == SDL_SCANCODE_R)
			m_Input.m_Reloading = State;

		for (int i = 0; i < NUM_CONTROLS; i++)
			if (currentEvent.key.scancode == ms_DefaultControls[i])
				m_Movement[i] = State;
	}
}

void Character::Tick()
{
	TickHealth();
	TickControls();      // Parse the inputs of each device keyboard/controller/AI
	TickProcessInputs(); // Do stuff depending on the current held buttons
	TickHook();          // Move hook and or player etc.
	TickCurrentWeapon(); // Shoot accelerate reload etc.
	m_Hands.Tick();
	m_ErrorStatuses.Tick();
//    TickErrorTimers(); // Ticks timer for errors
	// Need every characters to get here..
	// then we apply the accelerations of all
	// hooks and continue with the code below v v v
	// like collisions and velocity tick

	TickCollision();
	TickVelocity(); // Move the characters entity
	TickWalls();    // Check if colliding with walls

	// Check if health on screen needs updating
	if ((int)(m_HealthComponent.m_Health) != (int)(m_HealthComponent.m_LastHealth))
		m_HealthInt->FlagForUpdate();

	// Check if coordinate plate on screen needs updating
	if (m_World->GetNamesShown() > 0.05 &&
		((int)(m_Core.Pos.x) != (int)(m_LastCore.Pos.x) ||
			(int)(m_Core.Pos.y) != (int)(m_LastCore.Pos.y)))
		m_CoordinatePlate->FlagForUpdate();

	m_HitTicks -= 1;
	if (m_HitTicks < 0)
		m_HitTicks = 0;

	// Summarize the current tick
	m_HealthComponent.TickUpdateLastHealth();
	TickUpdateLastCore();
	memcpy(&m_LastInput, &m_Input, sizeof(CharacterInput));

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
