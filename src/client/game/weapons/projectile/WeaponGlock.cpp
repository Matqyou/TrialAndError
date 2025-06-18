//
// Created by T450s on 01/04/2023.
//

#include "WeaponGlock.h"
#include "../../entities/characters/character/Character.h"
#include "../../entities/Projectile.h"

LoadedTexture WeaponGlock::sTextureWeapon("weapons.glock");
LoadedTexture WeaponGlock::sTextureProjectile("entity.projectile.glock");
LoadedSound WeaponGlock::sShootSound("weapon.glock.shoot2");
LoadedSound WeaponGlock::sClickSound("weapon.glock.fail_reload");
LoadedSound WeaponGlock::sReloadSound("weapon.glock.reload");
Vec2d WeaponGlock::sHoldPosition(10.0, 0.0);
std::pair<Vec2d, Vec2d> WeaponGlock::sHandPositions = {{ 5.0, -5.0 }, { 10.0, 2.0 }};

WeaponGlock::WeaponGlock(DirectionalEntity *parent)
	: ProjectileWeapon(parent,
					   WEAPON_GLOCK,
					   sTextureWeapon.GetTexture(),
					   sReloadSound.GetSound(),
					   &sHoldPosition,
					   &sHandPositions,
					   10,
					   15,
					   15 * 3,
					   35.0, false)
{
	m_RecoilForce = 3.0;
	m_Damage = 7.5;
}

void WeaponGlock::Tick()
{
	if (m_Parent->GetType() != CHARACTER_ENTITY)
	{
		std::printf("Warning: Weapon holder is not a characters (no support for error powerups)");
		return;
	}

	TickTrigger();

	if (m_Parent && m_Triggered)
	{ // If want to trigger without an owner, need to save world somewhere
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

			Vec2d ProjectileVelocity = ShooterCore.Direction * m_ProjectileSpeed;
			new Projectile(World,
						   m_Parent,
						   WEAPON_GLOCK,
						   sTextureProjectile.GetTexture(),
						   m_Damage,
						   ShooterCore.Pos,
						   ProjectileVelocity);

			double recoil = ((Character *)m_Parent)->GetErrorStatuses().DangerousRecoil.IsActive() ? m_RecoilForce * 3.0
																								   : m_RecoilForce;
			Vec2d recoil_acceleration = ShooterCore.Direction * -recoil;
			m_Parent->Accelerate(recoil_acceleration);
		}
		else
		{
			sClickSound.GetSound()->PlaySound();
		}
	}
}