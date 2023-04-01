//
// Created by Matq on 30/03/2023.
//

#include "ProjectileWeapon.h"
#include "../entities/Character.h"
#include "../entities/Bullets.h"
#include <cmath>

double ProjectileWeapon::GenerateSpreadAngle() const {
    return (double(rand() % m_FullRandomSpread) - m_HalfRandomSpread) / m_RandomSpreadDivisor;
}

double ProjectileWeapon::GenerateRandomProjectileSpeed() const {
    return m_ProjectileSpeed + ((rand() % m_FullRandomProjectileSpeed)
        - m_NegativeRandomProjectileSpeed) / m_RandomProjectileSpeedDivisor;
}

ProjectileWeapon::ProjectileWeapon(Character* owner, int tick_cooldown, int ammo_capacity, double projectile_speed, bool automatic) {
    m_Owner = owner;
    m_LastShotAt = 0;
    m_TickCooldown = tick_cooldown;
    m_AmmoCapacity = ammo_capacity;
    m_Ammo = m_AmmoCapacity;
    m_ProjectileSpeed = projectile_speed;
    m_Automatic = automatic;
    m_Triggered = false;
    m_LastShot = false;

    m_RandomSpreadDivisor = 0.0;
    m_HalfRandomSpread = 0.0;
    m_FullRandomSpread = 0;
    m_RandomProjectileSpeedDivisor = 0.0;
    m_NegativeRandomProjectileSpeed = 0.0;
    m_FullRandomProjectileSpeed = 0;
}

void ProjectileWeapon::TickTrigger() {
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

void ProjectileWeapon::GetOwnerPosition(double& out_x, double& out_y, double& out_x_dir, double& out_y_dir) const {
    out_x = m_Owner->m_x;
    out_y = m_Owner->m_y;
    out_x_dir = m_Owner->m_xLook;
    out_y_dir = m_Owner->m_yLook;
}

void ProjectileWeapon::GetOwnerShooting(bool& out_shoot, bool& out_last_shoot) const {
    out_shoot = m_Owner->m_Shoot;
    out_last_shoot = m_Owner->m_LastShoot;
}

void ProjectileWeapon::SetSpread(double degrees, int decimal_places) {
    m_RandomSpreadDivisor = std::pow(10, decimal_places);
    m_HalfRandomSpread = degrees / 180.0 * M_PI * m_RandomSpreadDivisor;
    m_FullRandomSpread = int(m_HalfRandomSpread * 2.0) + 1;
}

void ProjectileWeapon::SetRandomProjectileSpeed(double delta_speed, double delta_percentage_negative, int delta_decimal_places) {
    m_RandomProjectileSpeedDivisor = std::pow(10, delta_decimal_places);
    double FullSpeed = delta_speed * m_RandomProjectileSpeedDivisor;
    m_NegativeRandomProjectileSpeed = FullSpeed * delta_percentage_negative;
    m_FullRandomProjectileSpeed = int(FullSpeed) + 1;
}

void ProjectileWeapon::Tick() {
    TickTrigger();
}
