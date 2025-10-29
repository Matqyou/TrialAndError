//
// Created by Matq on 30/03/2023.
//

#include "ProjectileWeapon.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/entities/cartesian/Projectile.h>
#include <cmath>

static LinkSound sNoAmmoSound("weapon.no_ammo");

double ProjectileWeapon::GenerateSpreadAngle() const
{
	return (double(rand() % m_FullRandomSpread) - m_HalfRandomSpread) / m_RandomSpreadDivisor;
}

double ProjectileWeapon::GenerateRandomProjectileSpeed() const
{
	return m_ProjectileSpeed + ((rand() % m_FullRandomProjectileSpeed)
		- m_NegativeRandomProjectileSpeed) / m_RandomProjectileSpeedDivisor;
}

ProjectileWeapon::ProjectileWeapon(DirectionalEntity *owner,
								   WeaponType type,
								   Texture *texture,
								   Sound *reload_sound, // Has default
								   Vec2d *hold_position, // Has default
								   std::pair<Vec2d, Vec2d> *hand_positions, // Has default
								   int tick_cooldown,
								   int ammo_capacity,
								   int total_ammo_capacity,
								   double projectile_speed,
								   bool automatic)
{
	m_Parent = owner;
	m_Type = type;
	m_Texture = texture;
	m_LastShotAt = 0;
	m_TickCooldown = tick_cooldown;
	m_AmmoCapacity = ammo_capacity;
	m_Ammo = m_AmmoCapacity;
	m_TrueAmmoCapacity = total_ammo_capacity;
	m_TrueAmmo = m_TrueAmmoCapacity;
	m_ProjectileSpeed = projectile_speed;
	m_Automatic = automatic;
	m_Triggered = false;
	m_LastShot = false;

	m_RandomSpreadDivisor = 0.0;
	m_HalfRandomSpread = 0.0;
	m_FullRandomSpread = 0;
	m_RandomProjectileSpeedDivisor = 0.0;
	m_NegativeRandomProjectileSpeed = 0.0;
	m_FullRandomProjectileSpeed = 0;
	m_Parent = nullptr;

	// Defaults
	m_ReloadSound = reload_sound == nullptr ? Assets.GetSound("weapon.default.reload") : reload_sound;
	m_HoldPosition = hold_position == nullptr ? Vec2d(5.0, 0.0) : *hold_position;
	m_LeftHandPosition = hand_positions == nullptr ? Vec2d(0, -10.0) : hand_positions->first;
	m_RightHandPosition = hand_positions == nullptr ? Vec2d(0, 10.0) : hand_positions->second;
}

void ProjectileWeapon::TickTrigger()
{
#ifndef NDEBUG
	if (m_Parent == nullptr)
	{
		throw std::runtime_error("ProjectileWeapon Parent is nullptr");
	}

	if (m_Parent->GetType() != CHARACTER_ENTITY)
	{
		throw std::runtime_error("ProjectileWeapon Parent is non character");
	}
#endif

	bool Shoot = ((Character *)(m_Parent))->m_Input.m_Shooting;
	bool LastShoot = ((Character *)(m_Parent))->m_LastInput.m_Shooting;

	m_Triggered = Shoot && !LastShoot;  // Always trigger on semi
	if (!m_Triggered)
	{
		bool Auto = Shoot && m_Automatic;
		m_Triggered = Auto && (m_Ammo || m_LastShot);
	}
}

unsigned int ProjectileWeapon::NeededTrueAmmo() const
{
	return m_TrueAmmoCapacity - m_TrueAmmo;
}

unsigned int ProjectileWeapon::AddMagAmmo(unsigned int count)
{
	m_Ammo += count;

	if (m_Ammo > m_AmmoCapacity)
	{
		auto Remaining = m_Ammo - m_AmmoCapacity;
		m_Ammo = m_AmmoCapacity;
		return Remaining;
	}

	return 0;
}

unsigned int ProjectileWeapon::AddTrueAmmo(unsigned int count)
{
	m_TrueAmmo += count;

	if (m_TrueAmmo > m_TrueAmmoCapacity)
	{
		auto Remaining = m_TrueAmmo - m_TrueAmmoCapacity;
		m_TrueAmmo = m_TrueAmmoCapacity;
		return Remaining;
	}

	return 0;
}

void ProjectileWeapon::SetParent(DirectionalEntity *parent)
{
	m_Parent = parent;
}

void ProjectileWeapon::SetSpread(double degrees, int decimal_places)
{
	m_RandomSpreadDivisor = std::pow(10, decimal_places);
	m_HalfRandomSpread = degrees / 180.0 * M_PI * m_RandomSpreadDivisor;
	m_FullRandomSpread = int(m_HalfRandomSpread * 2.0) + 1;
}

void ProjectileWeapon::SetRandomProjectileSpeed(double delta_speed,
												double delta_percentage_negative,
												int delta_decimal_places)
{
	m_RandomProjectileSpeedDivisor = std::pow(10, delta_decimal_places);
	double FullSpeed = delta_speed * m_RandomProjectileSpeedDivisor;
	m_NegativeRandomProjectileSpeed = FullSpeed * delta_percentage_negative;
	m_FullRandomProjectileSpeed = int(FullSpeed) + 1;
}

void ProjectileWeapon::Reload()
{
	if (m_TrueAmmo != 0)
	{
		m_ReloadSound->PlaySound();
	}
	else
	{
		sNoAmmoSound.GetSound()->PlaySound();
	}

	unsigned int AmmoNeeded = m_AmmoCapacity - m_Ammo;
	if (m_TrueAmmo >= AmmoNeeded)
	{
		m_TrueAmmo -= AmmoNeeded;
		m_Ammo = m_AmmoCapacity;
	}
	else
	{
		m_Ammo += m_TrueAmmo;
		m_TrueAmmo = 0;
	}
}

void ProjectileWeapon::Tick()
{
	TickTrigger();
}

void ProjectileWeapon::OnSelected()
{

}
