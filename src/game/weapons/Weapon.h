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
protected:
    double m_BulletSpeed;
    double m_RecoilForce;

public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    WeaponGlock(Character* owner);

    void Tick() override;
};

#endif //TRIALANDERROR_SRC_GAME_WEAPONS_WEAPON_H_
