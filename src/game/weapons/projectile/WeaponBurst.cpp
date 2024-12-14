//
// Created by T450s on 01/04/2023.
//

#include "WeaponBurst.h"
#include "../../entities/characters/character/Character.h"
#include "../../entities/Projectile.h"
#include <cmath>

LoadedTexture WeaponBurst::sTextureProjectile("entity.projectile.burst");
LoadedSound WeaponBurst::sShootSound("weapon.burst.shoot");
LoadedSound WeaponBurst::sClickSound("weapon.burst.fail_reload");
LoadedSound WeaponBurst::sReloadSound("weapon.burst.reload");
Vec2d WeaponBurst::sHoldPosition(5.0, 0.0);
std::pair<Vec2d, Vec2d> WeaponBurst::sHandPositions = {{ 60.0, -3.0 }, { 20.0, 2.0 }};

WeaponBurst::WeaponBurst(Character* owner)
    : ProjectileWeapon(owner,
                       WEAPON_BURST,
                       sReloadSound.GetSound(),
                       &sHoldPosition,
                       &sHandPositions,
                       30,
                       24,
                       24 * 3,
                       35.0,
                       false) {
    m_BaseRecoilForce = 3.0;
    m_RecoilForce = m_BaseRecoilForce;
    m_Damage = 8.5;
    m_BurstCooldown = 6;
    m_BurstShots = 3;

    // Will get rewritten by other code
    m_BurstTick = 0;
    m_BurstShotsLeft = 0;
}

void WeaponBurst::Tick() {
    if (m_Parent->GetType() != CHARACTER_ENTITY) {
        std::printf("Warning: Weapon holder is not a characters (no support for error powerups)");
        return;
    }

    TickTrigger();

    if (m_Parent) {
        GameWorld* World = m_Parent->World();
        auto& ShooterCore = m_Parent->GetDirectionalCore();
        auto CurrentTick = World->GetTick();
        if (m_BurstShotsLeft && CurrentTick - m_BurstTick > m_BurstCooldown) {
            m_BurstTick = CurrentTick;
            m_BurstShotsLeft--;
            if (m_Ammo) {
                m_Ammo--;
                sShootSound.GetSound()->PlaySound();

                Vec2d ProjectileVelocity = ShooterCore.Direction * m_ProjectileSpeed;
                new Projectile(World,
                               m_Parent,
                               WEAPON_BURST,
                               sTextureProjectile.GetTexture(),
                               m_Damage,
                               ShooterCore.Pos,
                               ProjectileVelocity);

                double RecoilX = ShooterCore.Direction.x * -m_RecoilForce;
                double RecoilY = ShooterCore.Direction.y * -m_RecoilForce;
                m_Parent->Accelerate(Vec2d(RecoilX, RecoilY));
            } else {
                sClickSound.GetSound()->PlaySound();
            }
        }

        if (m_Triggered) { // If want to trigger without an owner, need to save world somewhere
            if (CurrentTick - m_LastShotAt <= m_TickCooldown)
                return;

            m_BurstTick = CurrentTick;
            m_BurstShotsLeft = m_BurstShots - 1;
            m_LastShot = m_Ammo == 1;
            if (m_Ammo) {
                m_Ammo--;
                m_LastShotAt = CurrentTick;
                sShootSound.GetSound()->PlaySound();

                Vec2d ProjectileVelocity = ShooterCore.Direction * m_ProjectileSpeed;
                new Projectile(World,
                               m_Parent,
                               WEAPON_BURST,
                               sTextureProjectile.GetTexture(),
                               m_Damage,
                               ShooterCore.Pos,
                               ProjectileVelocity);

                double recoil = ((Character*)m_Parent)->GetErrorStatuses().DangerousRecoil.IsActive() ? m_RecoilForce * 3.0 : m_RecoilForce;
                Vec2d recoil_acceleration = ShooterCore.Direction * -recoil;
                m_Parent->Accelerate(recoil_acceleration);
            } else {
                sClickSound.GetSound()->PlaySound();
            }
        }
    }
}
