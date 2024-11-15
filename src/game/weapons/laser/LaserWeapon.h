//
// Created by Matq on 24/05/2023.
//

#pragma once

#include "../../GameWorld.h"

enum WeaponType {
    WEAPON_GLOCK,
    WEAPON_BURST,
    WEAPON_SHOTGUN,
    WEAPON_MINIGUN,
    NUM_WEAPONS
};

class Character;
class ProjectileWeapon {
protected:
    unsigned long long m_LastShotAt;
    unsigned long long m_TickCooldown;
    unsigned int m_Ammo, m_AmmoCapacity;
    unsigned int m_TrueAmmo, m_TrueAmmoCapacity;
    double m_ProjectileSpeed;
    bool m_Automatic;
    bool m_Triggered;
    bool m_LastShot;

    double m_RandomSpreadDivisor;
    double m_HalfRandomSpread;
    int m_FullRandomSpread;

    double m_RandomProjectileSpeedDivisor;
    double m_NegativeRandomProjectileSpeed;
    int m_FullRandomProjectileSpeed;

    Character* m_Parent;
    WeaponType m_Type;

    virtual void TickTrigger();
    void GetOwnerPosition(double& out_x, double& out_y, double& out_x_dir, double& out_y_dir) const;
    void GetOwnerShooting(bool& out_shoot, bool& out_last_shoot) const;
    double GenerateSpreadAngle() const;
    double GenerateRandomProjectileSpeed() const;
public:
    static Sound* ms_ReloadSound;
    static Sound* ms_NoAmmo;

    ProjectileWeapon(Character* owner,
                     WeaponType type,
                     int tick_cooldown,
                     int ammo_capacity,
                     int total_ammo_capacity,
                     double projectile_speed,
                     bool automatic);

    WeaponType WepType() const { return m_Type; }
    unsigned int GetMagAmmo() const { return m_Ammo; }
    unsigned int GetTrueAmmo() const { return m_TrueAmmo; }
    unsigned int AmmoCap() const { return m_AmmoCapacity; }
    unsigned int NeededTrueAmmo() const;
    unsigned long long TickCooldown() const { return m_TickCooldown; }

    void AddTrueAmmo(unsigned int count);
    void SetSpread(double degrees, int decimal_places);
    void SetRandomProjectileSpeed(double delta_speed, double delta_percentage_negative, int delta_decimal_places);
    void Reload();

    virtual void Tick();
};
