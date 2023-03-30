//
// Created by Matq on 30/03/2023.
//

#include "Weapon.h"
#include "../entities/Character.h"
#include "../entities/Bullets.h"

Weapon::Weapon(Character* owner, int tick_cooldown, int ammo_capacity, bool automatic) {
    m_LastShotAt = 0;
    m_TickCooldown = tick_cooldown;
    m_AmmoCapacity = ammo_capacity;
    m_Ammo = m_AmmoCapacity;
    m_Automatic = automatic;
    m_Triggered = false;
    m_LastShot = false;

    m_Owner = owner;
}

void Weapon::TickTrigger() {
    if (m_Owner) {
        m_Triggered = m_Owner->m_Shoot && !m_Owner->m_LastShoot;  // Always trigger on semi
        if (!m_Triggered) {
            bool Auto = m_Owner->m_Shoot && m_Automatic;
            m_Triggered = Auto && (m_Ammo || m_LastShot);
        }
    }
}

void Weapon::GetPosition(double& out_x, double& out_y, double& out_x_dir, double& out_y_dir) {
    out_x = m_Owner->m_x;
    out_y = m_Owner->m_y;
    out_x_dir = m_Owner->m_xLook;
    out_y_dir = m_Owner->m_yLook;
}

void Weapon::Tick() {
    TickTrigger();
}

Sound* WeaponGlock::ms_ShootSound = nullptr;
Sound* WeaponGlock::ms_ClickSound = nullptr;

WeaponGlock::WeaponGlock(Character* owner)
 : Weapon(owner, 10, 15, false) {
    m_BulletSpeed = 35.0;
    m_RecoilForce = 10.0;
}

void WeaponGlock::Tick() {
    TickTrigger();

    if (m_Owner && m_Triggered) { // If want to trigger without an owner, need to save world somewhere
        GameWorld* World = m_Owner->World();
        auto CurrentTick = World->CurrentTick();
        if (CurrentTick - m_LastShotAt < m_TickCooldown)
            return;

        SoundManager* SoundHandler = World->GameWindow()->SoundHandler();

        m_LastShot = m_Ammo == 1;
        if (m_Ammo) {
            m_Ammo--;
            m_LastShotAt = CurrentTick;
            SoundHandler->PlaySound(ms_ShootSound);

            double SpawnX, SpawnY, DirectionX, DirectionY;
            GetPosition(SpawnX, SpawnY, DirectionX, DirectionY);

            double VelocityX = DirectionX * m_BulletSpeed;
            double VelocityY = DirectionY * m_BulletSpeed;
            new Bullets(World, SpawnX, SpawnY, VelocityX, VelocityY);

            double RecoilX = DirectionX * -m_RecoilForce;
            double RecoilY = DirectionY * -m_RecoilForce;
            m_Owner->Accelerate(RecoilX, RecoilY);
        } else {
            SoundHandler->PlaySound(ms_ClickSound);
        }
    }
}