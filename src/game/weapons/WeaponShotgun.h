//
// Created by T450s on 01/04/2023.
//

#ifndef TRIALANDERROR_WEAPONSHOTGUN_H
#define TRIALANDERROR_WEAPONSHOTGUN_H

#include "ProjectileWeapon.h"

class WeaponShotgun : public ProjectileWeapon {
private:
    int m_PelletCount;
    double m_RecoilForce;
    double m_Damage;

public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    WeaponShotgun(Character* owner);

    void Tick() override;
};

#endif //TRIALANDERROR_WEAPONSHOTGUN_H
