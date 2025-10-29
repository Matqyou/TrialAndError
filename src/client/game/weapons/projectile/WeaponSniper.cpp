//
// Created by T450s on 01/04/2023.
//

#include "WeaponSniper.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/entities/cartesian/Projectile.h>

static LinkTexture sTextureWeapon("weapons.sniper");
static LinkTexture sTextureProjectile("entity.projectile.sniper");
static LinkSound sShootSound("weapon.shotgun.shoot2");
static LinkSound sClickSound("weapon.sniper.fail_reload");
static LinkSound sReloadSound("weapon.sniper.reload");
Vec2d WeaponSniper::sHoldPosition(1.0, 0.0);
std::pair<Vec2d, Vec2d> WeaponSniper::sHandPositions = {{ 80.0, -3.0 }, { 20.0, 2.0 }};

WeaponSniper::WeaponSniper(DirectionalEntity *parent)
	: ProjectileWeapon(parent,
					   WEAPON_SNIPER,
					   sTextureWeapon.GetTexture(),
					   sReloadSound.GetSound(),
					   &sHoldPosition,
					   &sHandPositions,
					   50,
					   8,
					   16,
					   100.0,
					   false)
{
	m_BaseRecoilForce = 12.0;
	m_RecoilForce = m_BaseRecoilForce;
	m_Damage = 70;
}

void WeaponSniper::Tick()
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
						   WEAPON_SNIPER,
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