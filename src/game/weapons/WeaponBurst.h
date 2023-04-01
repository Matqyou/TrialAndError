//
// Created by T450s on 01/04/2023.
//

#ifndef TRIALANDERROR_WEAPONBURST_H
#define TRIALANDERROR_WEAPONBURST_H

#include "ProjectileWeapon.h"

class WeaponBurst : public ProjectileWeapon {
private:
    double m_RecoilForce;

    unsigned long long m_BurstTick;
    int m_BurstCooldown;
    int m_BurstShots;
    int m_BurstShotsLeft;

public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    WeaponBurst(Character* owner);

    void Tick() override;
};

#endif //TRIALANDERROR_WEAPONBURST_H
