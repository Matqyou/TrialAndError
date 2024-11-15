//
// Created by T450s on 01/04/2023.
//

#include "WeaponShotgun.h"
#include "../../entities/characters/character/Character.h"
#include "../../entities/Projectile.h"
#include <cmath>

Sound* WeaponShotgun::ms_ShootSound = nullptr;
Sound* WeaponShotgun::ms_ClickSound = nullptr;

WeaponShotgun::WeaponShotgun(Character* owner)
    : ProjectileWeapon(owner, WEAPON_SHOTGUN, 42, 8, 8 * 3, 35.0, false) {
    m_PelletCount = 6;
    m_BaseRecoilForce = 20.0;
    m_RecoilForce = m_BaseRecoilForce;
    m_Damage = 6;

    SetSpread(10.0, 3);
    SetRandomProjectileSpeed(13.0, 0.5, 1);
}

void WeaponShotgun::Tick() {
    if (m_Parent->GetEntityType() != ENTTYPE_CHARACTER) {
        std::printf("Warning: Weapon holder is not a characters (no support for error powerups)");
        return;
    }

    // TODO: recoil force shouldn't change every tick (make like an event function, call when timer starts and ends to update recoil force)
    // Do this for the remaining weapons aswell
    if (!((Character*)m_Parent)->HasDangerousRecoil()) m_RecoilForce = m_BaseRecoilForce;
    else if (m_RecoilForce != m_BaseRecoilForce * 3) m_RecoilForce = m_BaseRecoilForce * 3;
    TickTrigger();

    if (m_Parent && m_Triggered) {  // If want to trigger without an owner, need to save world somewhere
        GameWorld* World = m_Parent->World();
        auto& ShooterCore = m_Parent->GetDirectionalCore();
        auto CurrentTick = World->GetTick();
        if (CurrentTick - m_LastShotAt <= m_TickCooldown)
            return;

        SoundManager* SoundHandler = World->GameWindow()->Assets()->SoundHandler();

        m_LastShot = m_Ammo == 1;
        if (m_Ammo) {
            m_Ammo--;
            m_LastShotAt = CurrentTick;
            SoundHandler->PlaySound(ms_ShootSound);

            double LookAngle = ShooterCore.Direction.Atan2();
            for (int i = 0; i < m_PelletCount; i++) {
                double ProjectileAngle = LookAngle + GenerateSpreadAngle();
                double ProjectileSpeed = GenerateRandomProjectileSpeed();
                Vec2d ProjectileVelocity = AngleVec2d(ProjectileAngle) * ProjectileSpeed;
                new Projectile(World,
                               m_Parent,
                               WEAPON_SHOTGUN,
                               m_Damage,
                               ShooterCore.Pos,
                               ProjectileVelocity);
            }

            double RecoilX = ShooterCore.Direction.x * -m_RecoilForce;
            double RecoilY = ShooterCore.Direction.y * -m_RecoilForce;
            m_Parent->Accelerate(Vec2d(RecoilX, RecoilY));
        } else {
            SoundHandler->PlaySound(ms_ClickSound);
        }
    }
}
