//
// Created by T450s on 01/04/2023.
//

#include "WeaponMinigun.h"
#include "../../entities/character/Character.h"
#include "../../entities/Bullets.h"
#include <cmath>

Sound* WeaponMinigun::ms_ShootSound = nullptr;
Sound* WeaponMinigun::ms_ClickSound = nullptr;

WeaponMinigun::WeaponMinigun(Character* owner)
    : ProjectileWeapon(owner, WEAPON_MINIGUN, 14, 64, 64 * 3, 35.0, true) {
    m_BaseRecoilForce = 3.2;
    m_RecoilForce = m_BaseRecoilForce;

    m_MinimumFireRate = 5.0;
    m_FullRate = 10.0;
    m_RateAcceleration = 0.07;
    m_RateDeacceleration = 0.05;

    // Will get rewritten by other code
    m_FireRate = 0.0;
    m_RandomSpreadDivisor = 0.0;
    m_HalfRandomSpread = 0.0;
    m_FullRandomSpread = 0;

    SetSpread(3.0, 2);
    SetRandomProjectileSpeed(5.0, 0.5, 2);
}

void WeaponMinigun::Tick() {
    if (!m_Shooter->GetIfDangerousRecoil())m_RecoilForce = m_BaseRecoilForce;
    else if (m_RecoilForce != m_BaseRecoilForce * 3)m_RecoilForce = m_BaseRecoilForce * 3;
    TickTrigger();

    if (!m_Shooter) return;
    GameWorld* World = m_Shooter->World();
    auto ShooterCore = (LookingEntityCore*) m_Shooter->GetCore();
    SoundManager* SoundHandler = World->GameWindow()->Assets()->SoundHandler();
    auto CurrentTick = World->GetTick();
    if (m_Triggered) { // If want to trigger without an owner, need to save world somewhere
        m_FireRate += m_RateAcceleration;
        if (m_FireRate > m_FullRate) m_FireRate = m_FullRate;
        if (m_FireRate < m_MinimumFireRate ||
            CurrentTick - m_LastShotAt <= int(m_TickCooldown - m_FireRate)) {}
        else {
            m_LastShot = m_Ammo == 1;
            if (m_Ammo) {
                m_Ammo--;
                m_LastShotAt = CurrentTick;
                SoundHandler->PlaySound(ms_ShootSound);

                double Angle = atan2(ShooterCore->m_ylook, ShooterCore->m_xlook) + GenerateSpreadAngle();

                double VelocityX = cos(Angle) * m_ProjectileSpeed;
                double VelocityY = sin(Angle) * m_ProjectileSpeed;
                new Bullets(World,
                            m_Shooter,
                            WEAPON_MINIGUN,
                            4,
                            ShooterCore->m_x,
                            ShooterCore->m_y,
                            VelocityX,
                            VelocityY);

                double RecoilX = ShooterCore->m_xlook * -m_RecoilForce;
                double RecoilY = ShooterCore->m_ylook * -m_RecoilForce;
                m_Shooter->Accelerate(RecoilX, RecoilY);
            } else {
                SoundHandler->PlaySound(ms_ClickSound);
            }
        }
    } else {
        m_FireRate -= m_RateDeacceleration;
        if (m_FireRate < 0.0) m_FireRate = 0.0;
    }
    m_Rotation += m_FireRate;
}