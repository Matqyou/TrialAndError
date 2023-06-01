//
// Created by Matq on 30/03/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_
#define TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_

#include "../../../GameWorld.h"

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

    Character* m_Shooter;
    WeaponType m_Type;

    virtual void TickTrigger();
    void GetOwnerShooting(bool& out_shoot, bool& out_last_shoot) const;
    double GenerateSpreadAngle() const;
    double GenerateRandomProjectileSpeed() const;
public:
    static Sound* ms_ReloadSound;
    static Sound* ms_NoAmmo;

    ProjectileWeapon(Character* owner, WeaponType type, int tick_cooldown, int ammo_capacity, int total_ammo_capacity, double projectile_speed, bool automatic);

    WeaponType Type() const { return m_Type; }
    unsigned int Ammo() const { return m_Ammo; }
    unsigned int TrueAmmo() const {return m_TrueAmmo;}
    unsigned int AmmoCap() const {return m_AmmoCapacity;}
    unsigned int NeededAmmo() const;
    unsigned long long TickCooldown() const { return m_TickCooldown; }
    bool IsAutomatic() const { return m_Automatic; }

    void AddTrueAmmo(unsigned int count);
    void SetSpread(double degrees, int decimal_places);
    void SetRandomProjectileSpeed(double delta_speed, double delta_percentage_negative, int delta_decimal_places);
    void Reload();

    virtual void Tick();
};

#endif //TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_
