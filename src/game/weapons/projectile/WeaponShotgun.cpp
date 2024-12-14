//
// Created by T450s on 01/04/2023.
//

#include "WeaponShotgun.h"
#include "../../entities/characters/character/Character.h"
#include "../../entities/Projectile.h"
#include <cmath>

LoadedTexture WeaponShotgun::sTextureWeapon("weapons.shotgun");
LoadedTexture WeaponShotgun::sTextureProjectile("entity.projectile.shotgun");
LoadedSound WeaponShotgun::sShootSound("weapon.shotgun.shoot2");
LoadedSound WeaponShotgun::sClickSound("weapon.shotgun.fail_reload");
LoadedSound WeaponShotgun::sReloadSound("weapon.shotgun.reload");
Vec2d WeaponShotgun::sHoldPosition(5.0, 0.0);
std::pair<Vec2d, Vec2d> WeaponShotgun::sHandPositions = {{ 60.0, -3.0 }, { 20.0, 2.0 }};

WeaponShotgun::WeaponShotgun(Character* owner)
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
                       false) {
    m_PelletCount = 6;
    m_BaseRecoilForce = 20.0;
    m_RecoilForce = m_BaseRecoilForce;
    m_Damage = 8;

    SetSpread(10.0, 3);
    SetRandomProjectileSpeed(13.0, 0.5, 1);
}

void WeaponShotgun::Tick() {
    if (m_Parent->GetType() != CHARACTER_ENTITY) {
        std::printf("Warning: Weapon holder is not a characters (no support for error powerups)");
        return;
    }

    TickTrigger();

    if (m_Parent && m_Triggered) {  // If want to trigger without an owner, need to save world somewhere
        GameWorld* World = m_Parent->World();
        auto& ShooterCore = m_Parent->GetDirectionalCore();
        auto CurrentTick = World->GetTick();
        if (CurrentTick - m_LastShotAt <= m_TickCooldown)
            return;

        m_LastShot = m_Ammo == 1;
        if (m_Ammo) {
            m_Ammo--;
            m_LastShotAt = CurrentTick;
            sShootSound.GetSound()->PlaySound();

            double LookAngle = ShooterCore.Direction.Atan2();
            for (int i = 0; i < m_PelletCount; i++) {
                double ProjectileAngle = LookAngle + GenerateSpreadAngle();
                double ProjectileSpeed = GenerateRandomProjectileSpeed();
                Vec2d ProjectileVelocity = AngleVec2d(ProjectileAngle) * ProjectileSpeed;
                new Projectile(World,
                               m_Parent,
                               WEAPON_SHOTGUN,
                               sTextureProjectile.GetTexture(),
                               m_Damage,
                               ShooterCore.Pos,
                               ProjectileVelocity);
            }

            double recoil = ((Character*)m_Parent)->GetErrorStatuses().DangerousRecoil.IsActive() ? m_RecoilForce * 3.0 : m_RecoilForce;
            Vec2d recoil_acceleration = ShooterCore.Direction * -recoil;
            m_Parent->Accelerate(recoil_acceleration);
        } else {
            sClickSound.GetSound()->PlaySound();
        }
    }
}
