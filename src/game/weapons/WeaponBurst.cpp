//
// Created by T450s on 01/04/2023.
//

#include "WeaponBurst.h"
#include "../entities/character/Character.h"
#include "../entities/Bullets.h"
#include <cmath>

Sound* WeaponBurst::ms_ShootSound = nullptr;
Sound* WeaponBurst::ms_ClickSound = nullptr;

WeaponBurst::WeaponBurst(Character* owner)
 : ProjectileWeapon(owner, WEAPON_BURST, 24, 18, 35.0, false) {
    m_RecoilForce = 8.0;
    m_BurstCooldown = 5;
    m_BurstShots = 3;

    // Will get rewritten by other code
    m_BurstTick = 0;
    m_BurstShotsLeft = 0;
}

void WeaponBurst::Tick() {
    TickTrigger();

    if (m_Owner) {
        GameWorld* World = m_Owner->World();
        SoundManager* SoundHandler = World->GameWindow()->Assets()->SoundHandler();
        auto CurrentTick = World->CurrentTick();
        if (m_BurstShotsLeft && CurrentTick - m_BurstTick > m_BurstCooldown) {
            m_BurstTick = CurrentTick;
            m_BurstShotsLeft--;
            if (m_Ammo) {
                m_Ammo--;
                SoundHandler->PlaySound(ms_ShootSound);

                double SpawnX, SpawnY, DirectionX, DirectionY;
                GetOwnerPosition(SpawnX, SpawnY, DirectionX, DirectionY);

                double VelocityX = DirectionX * m_ProjectileSpeed;
                double VelocityY = DirectionY * m_ProjectileSpeed;
                new Bullets(World, m_Owner, SpawnX, SpawnY, VelocityX, VelocityY);

                double RecoilX = DirectionX * -m_RecoilForce;
                double RecoilY = DirectionY * -m_RecoilForce;
                m_Owner->Accelerate(RecoilX, RecoilY);
            } else {
                SoundHandler->PlaySound(ms_ClickSound);
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
                SoundHandler->PlaySound(ms_ShootSound);

                double SpawnX, SpawnY, DirectionX, DirectionY;
                GetOwnerPosition(SpawnX, SpawnY, DirectionX, DirectionY);

                double VelocityX = DirectionX * m_ProjectileSpeed;
                double VelocityY = DirectionY * m_ProjectileSpeed;
                new Bullets(World, m_Owner, SpawnX, SpawnY, VelocityX, VelocityY);

                double RecoilX = DirectionX * -m_RecoilForce;
                double RecoilY = DirectionY * -m_RecoilForce;
                m_Owner->Accelerate(RecoilX, RecoilY);
            } else {
                SoundHandler->PlaySound(ms_ClickSound);
            }
        }
    }
}
