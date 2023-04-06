//
// Created by T450s on 01/04/2023.
//

#include "WeaponShotgun.h"
#include "../entities/Character.h"
#include "../entities/Bullets.h"
#include <cmath>

Sound* WeaponShotgun::ms_ShootSound = nullptr;
Sound* WeaponShotgun::ms_ClickSound = nullptr;

WeaponShotgun::WeaponShotgun(Character* owner)
 : ProjectileWeapon(owner, 64, 8, 35.0, false) {
    m_PelletCount = 6;
    m_RecoilForce = 25.0;

    SetSpread(12.0, 3);
    SetRandomProjectileSpeed(13.0, 0.5, 1);
}

void WeaponShotgun::Tick() {
    TickTrigger();

    if (m_Owner && m_Triggered) {  // If want to trigger without an owner, need to save world somewhere
        GameWorld* World = m_Owner->World();
        auto CurrentTick = World->CurrentTick();
        if (CurrentTick - m_LastShotAt <= m_TickCooldown)
            return;

        SoundManager* SoundHandler = World->GameWindow()->SoundHandler();

        m_LastShot = m_Ammo == 1;
        if (m_Ammo) {
            m_Ammo--;
            m_LastShotAt = CurrentTick;
            SoundHandler->PlaySound(ms_ShootSound);

            double SpawnX, SpawnY, DirectionX, DirectionY;
            GetOwnerPosition(SpawnX, SpawnY, DirectionX, DirectionY);

            double LookAngle = atan2(DirectionY, DirectionX);
            for (int i = 0; i < m_PelletCount; i++) {
                double ProjectileAngle = LookAngle + GenerateSpreadAngle();
                double ProjectileSpeed = GenerateRandomProjectileSpeed();
                double VelocityX = cos(ProjectileAngle) * ProjectileSpeed;
                double VelocityY = sin(ProjectileAngle) * ProjectileSpeed;

                new Bullets(World, SpawnX, SpawnY, VelocityX, VelocityY);
            }

            double RecoilX = DirectionX * -m_RecoilForce;
            double RecoilY = DirectionY * -m_RecoilForce;
            m_Owner->Accelerate(RecoilX, RecoilY);
        } else {
            SoundHandler->PlaySound(ms_ClickSound);
        }
    }
}
