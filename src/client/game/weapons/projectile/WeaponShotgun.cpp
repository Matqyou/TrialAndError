//
// Created by T450s on 01/04/2023.
//

#include "WeaponShotgun.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/entities/cartesian/Projectile.h>
#include <cmath>

static LinkTexture sTextureWeapon("weapons.shotgun");
static LinkTexture sTextureProjectile("entity.projectile.shotgun");
static LinkSound sShootSound("weapon.shotgun.shoot2");
static LinkSound sClickSound("weapon.shotgun.fail_reload");
static LinkSound sReloadSound("weapon.shotgun.reload");
Vec2f WeaponShotgun::sHoldPosition(5.0, 0.0);
std::pair<Vec2f, Vec2f> WeaponShotgun::sHandPositions = {{ 60.0, -3.0 }, { 20.0, 2.0 }};

WeaponShotgun::WeaponShotgun(Character *owner)
	: ProjectileWeapon(owner,
					   WEAPON_SHOTGUN,
					   sTextureWeapon.GetTexture(),
					   sReloadSound.GetSound(),
					   &sHoldPosition,
					   &sHandPositions,
					   42,
					   8,
					   8 * 3,
					   35.0,
					   false)
{
	m_PelletCount = 6;
	m_BaseRecoilForce = 20.0;
	m_RecoilForce = m_BaseRecoilForce;
	m_Damage = 8;

	SetSpread(10.0, 3);
	SetRandomProjectileSpeed(13.0, 0.5, 1);
}

void WeaponShotgun::Tick()
{
	if (m_Parent->GetType() != ENTITY_CHARACTER)
	{
		std::printf("Warning: Weapon holder is not a characters (no support for error powerups)");
		return;
	}

	TickTrigger();

	if (m_Parent && m_Triggered)
	{  // If want to trigger without an owner, need to save world somewhere
		GameWorld *World = m_Parent->World();
		auto& ShooterCore = m_Parent->GetDirectionalCore();
		auto CurrentTick = World->GetTick();
		if (CurrentTick - m_LastShotAt <= m_TickCooldown)
			return;

		m_LastShot = m_Ammo == 1;
		if (m_Ammo)
		{
			m_Ammo--;
			m_LastShotAt = CurrentTick;
			sShootSound.GetSound()->PlaySound();

			float LookAngle = ShooterCore.direction.Atan2();
			for (int i = 0; i < m_PelletCount; i++)
			{
				float ProjectileAngle = LookAngle + GenerateSpreadAngle();
				float ProjectileSpeed = GenerateRandomProjectileSpeed();
				Vec2f ProjectileVelocity = FromAngleVec2f(ProjectileAngle) * ProjectileSpeed;
				auto new_projectile = new Projectile(m_Parent,
							   WEAPON_SHOTGUN,
							   sTextureProjectile.GetTexture(),
							   m_Damage,
							   ShooterCore.pos,
							   ProjectileVelocity);
				World->AddEntity(new_projectile, true);
			}

			float recoil = ((Character *)m_Parent)->GetErrorStatuses().DangerousRecoil.IsActive() ?
						   m_RecoilForce * 3.0f : m_RecoilForce;
			Vec2f recoil_acceleration = ShooterCore.direction * -recoil;
			m_Parent->Accelerate(recoil_acceleration);
		}
		else
		{
			sClickSound.GetSound()->PlaySound();
		}
	}
}
