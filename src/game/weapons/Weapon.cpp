//
// Created by Matq on 30/03/2023.
//

#include "Weapon.h"
#include "../entities/Character.h"
#include "../entities/Bullets.h"
#include <cmath>

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
        bool Shoot, LastShoot;
        GetOwnerShooting(Shoot, LastShoot);

        m_Triggered = Shoot && !LastShoot;  // Always trigger on semi
        if (!m_Triggered) {
            bool Auto = Shoot && m_Automatic;
            m_Triggered = Auto && (m_Ammo || m_LastShot);
        }
    }
}

void Weapon::GetOwnerPosition(double& out_x, double& out_y, double& out_x_dir, double& out_y_dir) {
    out_x = m_Owner->m_x;
    out_y = m_Owner->m_y;
    out_x_dir = m_Owner->m_xLook;
    out_y_dir = m_Owner->m_yLook;
}

void Weapon::GetOwnerShooting(bool& out_shoot, bool& out_last_shoot) {
    out_shoot = m_Owner->m_Shoot;
    out_last_shoot = m_Owner->m_LastShoot;
}

void Weapon::Tick() {
    TickTrigger();
}

Sound* WeaponGlock::ms_ShootSound = nullptr;
Sound* WeaponGlock::ms_ClickSound = nullptr;

WeaponGlock::WeaponGlock(Character* owner)
 : Weapon(owner, 10, 15, false) {
    m_ProjectileSpeed = 35.0;
    m_RecoilForce = 10.0;
}

void WeaponGlock::Tick() {
    TickTrigger();

    if (m_Owner && m_Triggered) { // If want to trigger without an owner, need to save world somewhere
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

            double VelocityX = DirectionX * m_ProjectileSpeed;
            double VelocityY = DirectionY * m_ProjectileSpeed;
            new Bullets(World, SpawnX, SpawnY, VelocityX, VelocityY);

            double RecoilX = DirectionX * -m_RecoilForce;
            double RecoilY = DirectionY * -m_RecoilForce;
            m_Owner->Accelerate(RecoilX, RecoilY);
        } else {
            SoundHandler->PlaySound(ms_ClickSound);
        }
    }
}

Sound* WeaponShotgun::ms_ShootSound = nullptr;
Sound* WeaponShotgun::ms_ClickSound = nullptr;

double WeaponShotgun::GenerateSpreadAngle() const {
    return (double(rand() % m_FullRandomSpread) - m_HalfRandomSpread) / m_RandomSpreadDivisor;
}

double WeaponShotgun::GenerateRandomPelletSpeed() const {
    return m_PelletSpeed + ((rand() % m_FullRandomPelletSpeed) - m_NegativeRandomPelletSpeed) / m_RandomPelletSpeedDivisor;
}

WeaponShotgun::WeaponShotgun(Character* owner)
 : Weapon(owner, 64, 8, false) {
    m_PelletCount = 6;
    m_RecoilForce = 25.0;

    // Will be rewritten by functions
    m_PelletSpeed = 0.0;
    m_RandomSpreadDivisor = 1.0;
    m_HalfRandomSpread = 0.0;
    m_FullRandomSpread = 1;
    m_RandomPelletSpeedDivisor = 1.0;
    m_NegativeRandomPelletSpeed = 0.0;
    m_FullRandomPelletSpeed = 1;

    SetSpread(12.0, 3);
    SetPelletSpeeds(35.0, 13.0, 0.5, 1);
}

void WeaponShotgun::SetSpread(double degrees, int decimal_places) {
    m_RandomSpreadDivisor = std::pow(10, decimal_places);
    m_HalfRandomSpread = degrees / 180.0 * M_PI * m_RandomSpreadDivisor;
    m_FullRandomSpread = int(m_HalfRandomSpread * 2.0) + 1;
}

void WeaponShotgun::SetRandomPelletSpeed(double delta_speed, double delta_percentage_negative, int delta_decimal_places) {
    m_RandomPelletSpeedDivisor = std::pow(10, delta_decimal_places);
    double FullSpeed = delta_speed * m_RandomPelletSpeedDivisor;
    m_NegativeRandomPelletSpeed = FullSpeed * delta_percentage_negative;
    m_FullRandomPelletSpeed = int(FullSpeed) + 1;
}

void WeaponShotgun::SetPelletSpeeds(double base_speed, double delta_speed, double delta_percentage_negative, int delta_decimal_places) {
    m_PelletSpeed = base_speed;
    SetRandomPelletSpeed(delta_speed, delta_percentage_negative, delta_decimal_places);
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
                double ProjectileSpeed = GenerateRandomPelletSpeed();
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

Sound* WeaponBurst::ms_ShootSound = nullptr;
Sound* WeaponBurst::ms_ClickSound = nullptr;

WeaponBurst::WeaponBurst(Character* owner)
 : Weapon(owner, 24, 18, false) {
    m_ProjectileSpeed = 35.0;
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
        SoundManager* SoundHandler = World->GameWindow()->SoundHandler();
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
                new Bullets(World, SpawnX, SpawnY, VelocityX, VelocityY);

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
                new Bullets(World, SpawnX, SpawnY, VelocityX, VelocityY);

                double RecoilX = DirectionX * -m_RecoilForce;
                double RecoilY = DirectionY * -m_RecoilForce;
                m_Owner->Accelerate(RecoilX, RecoilY);
            } else {
                SoundHandler->PlaySound(ms_ClickSound);
            }
        }
    }
}