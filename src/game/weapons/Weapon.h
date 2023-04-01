//
// Created by Matq on 30/03/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_
#define TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_

#include "../../GameWorld.h"

class Character;
class Weapon {
protected:
    unsigned long long m_LastShotAt;
    unsigned int m_TickCooldown;
    unsigned int m_AmmoCapacity;
    unsigned int m_Ammo;
    bool m_Automatic;
    bool m_Triggered;
    bool m_LastShot;  // Set to true if last shot was shot in the previous shot

    Character* m_Owner;

    void TickTrigger();
    void GetPosition(double& out_x, double& out_y, double& out_x_dir, double& out_y_dir);

public:
    Weapon(Character* owner, int tick_cooldown, int ammo_capacity, bool automatic);

    Character* Owner() const { return m_Owner; }

    virtual void Tick();
};

class WeaponGlock : public Weapon {
private:
    double m_ProjectileSpeed;
    double m_RecoilForce;

public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    WeaponGlock(Character* owner);

    void Tick() override;
};

class WeaponShotgun : public Weapon {
private:
    int m_PelletCount;
    double m_PelletSpeed;
    double m_RecoilForce;

    double m_RandomSpreadDivisor;
    double m_HalfRandomSpread;
    int m_FullRandomSpread;
    double m_RandomPelletSpeedDivisor;
    double m_NegativeRandomPelletSpeed;
    int m_FullRandomPelletSpeed;

    double GenerateSpreadAngle() const;
    double GenerateRandomPelletSpeed() const;
public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    WeaponShotgun(Character* owner);

    void SetSpread(double degrees, int decimal_places);
    void SetRandomPelletSpeed(double delta_speed, double delta_percentage_negative, int delta_decimal_places);
    void SetPelletSpeeds(double base_speed, double delta_speed, double delta_percentage_negative, int delta_decimal_places);

    void Tick() override;
};

#endif //TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_
