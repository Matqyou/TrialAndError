//
// Created by T450s on 01/04/2023.
//

#include "WeaponMinigun.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/entities/cartesian/Projectile.h>
#include <cmath>

static LoadTexture sTextureWeapon[4] = {
	LoadTexture("weapons.minigun.1", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("weapons.minigun.2", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("weapons.minigun.3", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("weapons.minigun.4", AssetsClass::TexturePurpose::GUI_ELEMENT),
};
static LoadTexture sTextureProjectile("entity.projectile.minigun", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LinkSound sClickSound("weapon.minigun.fail_reload");
static LinkSound sReloadSound("weapon.minigun.reload");
static LinkSound sShootSound("weapon.minigun.shoot");
Vec2f WeaponMinigun::sHoldPosition(10.0, 0.0);
std::pair<Vec2f, Vec2f> WeaponMinigun::sHandPositions = {{ 10.0, -15.0 }, { 17.0, 13.0 }};

WeaponMinigun::WeaponMinigun(Character *owner)
	: ProjectileWeapon(owner,
					   WEAPON_MINIGUN,
					   sTextureWeapon[0].GetTexture(),
					   sReloadSound.GetSound(),
					   &sHoldPosition,
					   &sHandPositions,
					   14,
					   255,
					   255 * 2,
					   35.0,
					   true)
{
	m_BaseRecoilForce = 3.2;
	m_RecoilForce = m_BaseRecoilForce;
	m_Damage = 16.0;

	m_MinimumFireRate = 7.5;
	m_FullRate = 10.0;
	m_RateAcceleration = 0.07;
	m_RateDeacceleration = 0.03;

	// Will get rewritten by other code
	m_FireRate = 0.0;
	m_RandomSpreadDivisor = 0.0;
	m_HalfRandomSpread = 0.0;
	m_FullRandomSpread = 0;

	SetSpread(3.0, 2);
	SetRandomProjectileSpeed(5.0, 0.5, 2);
}

void WeaponMinigun::Tick()
{
	if (m_Parent->GetType() != ENTITY_CHARACTER)
	{
		std::printf("Warning: Weapon holder is not a characters (no support for error powerups)");
		return;
	}

	SetTexture(sTextureWeapon[int(std::fmod(m_Rotation, 100.0) / 25.0)].GetTexture());
	TickTrigger();

	if (!m_Parent) return;
	GameWorld *World = m_Parent->World();
	auto& ShooterCore = m_Parent->GetDirectionalCore();
	auto CurrentTick = World->GetTick();
	if (m_Triggered)
	{ // If want to trigger without an owner, need to save world somewhere
		m_FireRate += m_RateAcceleration;
		if (m_FireRate > m_FullRate) m_FireRate = m_FullRate;
		if (m_FireRate >= m_MinimumFireRate &
			CurrentTick - m_LastShotAt > int(double(m_TickCooldown) - m_FireRate))
		{
			m_LastShot = m_Ammo == 1;
			if (m_Ammo)
			{
				m_Ammo--;
				m_LastShotAt = CurrentTick;
				sShootSound.GetSound()->PlaySound();

				// todo: 3d
//				float Angle = ShooterCore.direction.Atan2F() + GenerateSpreadAngle();
//				Vec3f ProjectileVelocity = FromAngleVec2f(Angle) * m_ProjectileSpeed;
//				auto new_projectile = new Projectile(m_Parent,
//							   WEAPON_MINIGUN,
//							   sTextureProjectile.GetTexture(),
//							   m_Damage,
//							   ShooterCore.pos,
//							   ProjectileVelocity);
//				World->AddEntity(new_projectile, true);

				float recoil = ((Character *)m_Parent)->GetErrorStatuses().DangerousRecoil.IsActive() ?
							   m_RecoilForce * 3.0f : m_RecoilForce;
				Vec3f recoil_acceleration = ShooterCore.orientation.GetLook() * -recoil;
				m_Parent->Accelerate(recoil_acceleration);
			}
			else
			{
				sClickSound.GetSound()->PlaySound();
			}
		}
	}
	else
	{
		m_FireRate -= m_RateDeacceleration;
		if (m_FireRate < 0.0) m_FireRate = 0.0;
	}
	m_Rotation += m_FireRate;
}

void WeaponMinigun::Reload()
{
	ProjectileWeapon::Reload();
	m_FireRate = 0.0;
}

void WeaponMinigun::OnSelected()
{
	m_FireRate = 0.0;
}
