//
// Created by T450s on 01/04/2023.
//

#include "PatersonNavy.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/entities/cartesian/Projectile.h>

static LinkTexture sTextureWeapon("weapons.paterson_navy");
static LinkTexture sTextureProjectile("entity.projectile.paterson_navy");
static LinkSound sShootSound("weapon.paterson_navy.shoot");
static LinkSound sClickSound("weapon.paterson_navy.fail_reload");
static LinkSound sReloadSound("weapon.paterson_navy.reload");
Vec2f PatersonNavy::sHoldPosition(10.0, 0.0);
std::pair<Vec2f, Vec2f> PatersonNavy::sHandPositions = {{ 5.0, -5.0 }, { 10.0, 2.0 }};

PatersonNavy::PatersonNavy(DirectionalEntity *parent)
	: ProjectileWeapon(parent,
					   WEAPON_PATERSONNAVY,
					   sTextureWeapon.GetTexture(),
					   sReloadSound.GetSound(),
					   &sHoldPosition,
					   &sHandPositions,
					   60,
					   6,
					   6 * 6,
					   45.0f,
					   false)
{
	m_BaseRecoilForce = 5.0;
	m_RecoilForce = m_BaseRecoilForce;
	m_Damage = 40.0;
}

void PatersonNavy::Tick()
{
	if (m_Parent->GetType() != ENTITY_CHARACTER)
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

			Vec2f ProjectileVelocity = ShooterCore.direction * m_ProjectileSpeed;
			auto new_projectile = new Projectile(m_Parent,
						   WEAPON_GLOCK,
						   sTextureProjectile.GetTexture(),
						   m_Damage,
						   ShooterCore.pos,
						   ProjectileVelocity);
			World->AddEntity(new_projectile, true);

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