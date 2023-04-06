//
// Created by T450s on 01/04/2023.
//

#include "WeaponMinigun.h"
#include "../entities/Character.h"
#include "../entities/Bullets.h"
#include <cmath>

Sound* WeaponMinigun::ms_ShootSound = nullptr;
Sound* WeaponMinigun::ms_ClickSound = nullptr;

WeaponMinigun::WeaponMinigun(Character* owner)
 : ProjectileWeapon(owner, 14, 64, 35.0, true) {
    m_RecoilForce = 4.0;

    m_FullRate = 10.0;
    m_RateAcceleration = 0.07;
    m_RateDeacceleration = 0.4;

    // Will get rewritten by other code
    m_ShootRate = 0.0;
    m_RandomSpreadDivisor = 0.0;
    m_HalfRandomSpread = 0.0;
    m_FullRandomSpread = 0;

    SetSpread(3.0, 2);
    SetRandomProjectileSpeed(5.0, 0.5, 2);
}

void WeaponMinigun::Tick() {
    TickTrigger();

    if (m_Owner) {
        GameWorld* World = m_Owner->World();
        SoundManager* SoundHandler = World->GameWindow()->SoundHandler();
        auto CurrentTick = World->CurrentTick();

        if (m_Triggered) { // If want to trigger without an owner, need to save world somewhere
            m_ShootRate += m_RateAcceleration;
            if (m_ShootRate > m_FullRate) m_ShootRate = m_FullRate;
            if (CurrentTick - m_LastShotAt <= int(m_TickCooldown - m_ShootRate))
                return;

            m_LastShot = m_Ammo == 1;
            if (m_Ammo) {
                m_Ammo--;
                m_LastShotAt = CurrentTick;
                SoundHandler->PlaySound(ms_ShootSound);

                double SpawnX, SpawnY, DirectionX, DirectionY;
                GetOwnerPosition(SpawnX, SpawnY, DirectionX, DirectionY);

                double Angle = atan2(DirectionY, DirectionX) + GenerateSpreadAngle();

                double VelocityX = cos(Angle) * m_ProjectileSpeed;
                double VelocityY = sin(Angle) * m_ProjectileSpeed;
                new Bullets(World, SpawnX, SpawnY, VelocityX, VelocityY);

                double RecoilX = DirectionX * -m_RecoilForce;
                double RecoilY = DirectionY * -m_RecoilForce;
                m_Owner->Accelerate(RecoilX, RecoilY);
            } else {
                SoundHandler->PlaySound(ms_ClickSound);
            }
        } else {
            m_ShootRate -= m_RateDeacceleration;
            if (m_ShootRate < 0.0) m_ShootRate = 0.0;
        }
    }
}