//
// Created by T450s on 01/04/2023.
//

#include "WeaponMinigun.h"
#include "../../entities/characters/character/Character.h"
#include "../../entities/Projectile.h"
#include <cmath>

LoadedSound WeaponMinigun::sShootSound("weapon.minigun.shoot");
LoadedSound WeaponMinigun::sClickSound("weapon.minigun.fail_reload");
LoadedSound WeaponMinigun::sReloadSound("weapon.minigun.reload");
Vec2d WeaponMinigun::sHoldPosition(10.0, 0.0);
std::pair<Vec2d, Vec2d> WeaponMinigun::sHandPositions = {{ 10.0, -15.0 }, { 17.0, 13.0 }};

WeaponMinigun::WeaponMinigun(Character* owner)
    : ProjectileWeapon(owner,
                       WEAPON_MINIGUN,
                       sReloadSound.GetSound(),
                       &sHoldPosition,
                       &sHandPositions,
                       14,
                       255,
                       255 * 2,
                       35.0,
                       true) {
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

void WeaponMinigun::Tick() {
    if (m_Parent->GetType() != CHARACTER_ENTITY) {
        std::printf("Warning: Weapon holder is not a characters (no support for error powerups)");
        return;
    }

    // TODO: recoil force shouldn't change every tick (make like an event function, call when timer starts and ends to update recoil force)
    // Do this for the remaining weapons aswell
    if (!((Character*)m_Parent)->GetErrorStatuses().DangerousRecoil.IsActive()) m_RecoilForce = m_BaseRecoilForce;
    else if (m_RecoilForce != m_BaseRecoilForce * 3) m_RecoilForce = m_BaseRecoilForce * 3;
    TickTrigger();

    if (!m_Parent) return;
    GameWorld* World = m_Parent->World();
    auto& ShooterCore = m_Parent->GetDirectionalCore();
    auto CurrentTick = World->GetTick();
    if (m_Triggered) { // If want to trigger without an owner, need to save world somewhere
        m_FireRate += m_RateAcceleration;
        if (m_FireRate > m_FullRate) m_FireRate = m_FullRate;
        if (m_FireRate >= m_MinimumFireRate &
            CurrentTick - m_LastShotAt > int(double(m_TickCooldown) - m_FireRate)) {
            m_LastShot = m_Ammo == 1;
            if (m_Ammo) {
                m_Ammo--;
                m_LastShotAt = CurrentTick;
                sShootSound.GetSound()->PlaySound();

                double Angle = ShooterCore.Direction.Atan2() + GenerateSpreadAngle();
                Vec2d ProjectileVelocity = AngleVec2d(Angle) * m_ProjectileSpeed;
                new Projectile(World,
                               m_Parent,
                               WEAPON_MINIGUN,
                               m_Damage,
                               ShooterCore.Pos,
                               ProjectileVelocity);

                Vec2d Recoil = ShooterCore.Direction * -m_RecoilForce;
                m_Parent->Accelerate(Recoil);
            } else {
                sClickSound.GetSound()->PlaySound();
            }
        }
    } else {
        m_FireRate -= m_RateDeacceleration;
        if (m_FireRate < 0.0) m_FireRate = 0.0;
    }
    m_Rotation += m_FireRate;
}

void WeaponMinigun::Reload() {
    ProjectileWeapon::Reload();
    m_FireRate = 0.0;
}

void WeaponMinigun::OnSelected() {
    m_FireRate = 0.0;
}
