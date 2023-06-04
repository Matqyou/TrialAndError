//
// Created by T450s on 01/04/2023.
//

#include "WeaponShotgun.h"
#include "../../entities/character/Character.h"
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
    if (!m_Shooter->HasDangerousRecoil())m_RecoilForce = m_BaseRecoilForce;
    else if (m_RecoilForce != m_BaseRecoilForce * 3)m_RecoilForce = m_BaseRecoilForce * 3;
    TickTrigger();

    if (m_Shooter && m_Triggered) {  // If want to trigger without an owner, need to save world somewhere
        GameWorld* World = m_Shooter->World();
        auto& ShooterCore = m_Shooter->GetDirectionalCore();
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
                double VelocityX = cos(ProjectileAngle) * ProjectileSpeed;
                double VelocityY = sin(ProjectileAngle) * ProjectileSpeed;

                new Projectile(World,
                               m_Shooter,
                               WEAPON_SHOTGUN,
                               m_Damage,
                               ShooterCore.Pos.x,
                               ShooterCore.Pos.y,
                               VelocityX,
                               VelocityY);
            }

            double RecoilX = ShooterCore.Direction.x * -m_RecoilForce;
            double RecoilY = ShooterCore.Direction.y * -m_RecoilForce;
            m_Shooter->Accelerate(RecoilX, RecoilY);
        } else {
            SoundHandler->PlaySound(ms_ClickSound);
        }
    }
}
