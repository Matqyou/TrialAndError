//
// Created by Matq on 30/03/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_
#define TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_

#include "../../GameWorld.h"

enum WeaponType {
    WEAPON_GLOCK,
    WEAPON_BURST,
    WEAPON_SHOTGUN,
    WEAPON_MACHINEGUN,
    NUM_WEAPONS
};

class Character;
class ProjectileWeapon {
protected:
    unsigned long long m_LastShotAt;
    unsigned int m_TickCooldown;
    unsigned int m_AmmoCapacity;
    unsigned int m_Ammo;
    unsigned int m_TrueAmmo;
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

    Character* m_Owner;
    WeaponType m_Type;

    virtual void TickTrigger();
    void GetOwnerPosition(double& out_x, double& out_y, double& out_x_dir, double& out_y_dir) const;
    void GetOwnerShooting(bool& out_shoot, bool& out_last_shoot) const;
    double GenerateSpreadAngle() const;
    double GenerateRandomProjectileSpeed() const;
public:
    static Sound* ms_ReloadSound;
    static Sound* ms_NoAmmo;

    ProjectileWeapon(Character* owner, WeaponType type, int tick_cooldown, int ammo_capacity, double projectile_speed, bool automatic);

    WeaponType Type() const { return m_Type; }
    unsigned  int Ammo() const { return m_Ammo; }
    unsigned int TrueAmmo() const {return m_TrueAmmo;}
    unsigned  int AmmoCap() const {return m_AmmoCapacity;}
    void SetSpread(double degrees, int decimal_places);
    double SetAmmo(double count);
    void SetRandomProjectileSpeed(double delta_speed, double delta_percentage_negative, int delta_decimal_places);
    void Reload();
    virtual void Tick();
};

#endif //TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_
