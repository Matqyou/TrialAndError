//
// Created by T450s on 01/04/2023.
//

#include "WeaponGlock.h"
#include "../entities/character/Character.h"
#include "../entities/Bullets.h"

Sound* WeaponGlock::ms_ShootSound = nullptr;
Sound* WeaponGlock::ms_ClickSound = nullptr;

WeaponGlock::WeaponGlock(Character* owner)
 : ProjectileWeapon(owner, WEAPON_GLOCK, 10, 15, 35.0, false) {
    m_RecoilForce = 10.0;
}

void WeaponGlock::Tick() {
    TickTrigger();

    if (m_Owner && m_Triggered) { // If want to trigger without an owner, need to save world somewhere
        GameWorld *World = m_Owner->World();
        auto CurrentTick = World->CurrentTick();
        if (CurrentTick - m_LastShotAt <= m_TickCooldown)
            return;

        SoundManager *SoundHandler = World->GameWindow()->Assets()->SoundHandler();

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