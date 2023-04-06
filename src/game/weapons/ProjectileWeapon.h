//
// Created by Matq on 30/03/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_
#define TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_

#include "../../GameWorld.h"

class Character;
class ProjectileWeapon {
protected:
    unsigned long long m_LastShotAt;
    unsigned int m_TickCooldown;
    unsigned int m_AmmoCapacity;
    unsigned int m_Ammo;
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

    virtual void TickTrigger();
    void GetOwnerPosition(double& out_x, double& out_y, double& out_x_dir, double& out_y_dir) const;
    void GetOwnerShooting(bool& out_shoot, bool& out_last_shoot) const;
    double GenerateSpreadAngle() const;
    double GenerateRandomProjectileSpeed() const;
public:
    ProjectileWeapon(Character* owner, int tick_cooldown, int ammo_capacity, double projectile_speed, bool automatic);

    void SetSpread(double degrees, int decimal_places);
    void SetRandomProjectileSpeed(double delta_speed, double delta_percentage_negative, int delta_decimal_places);
    void Reload(){m_Ammo = m_AmmoCapacity;}
    virtual void Tick();
};

#endif //TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_
