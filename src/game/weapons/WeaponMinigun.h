//
// Created by T450s on 01/04/2023.
//

#ifndef TRIALANDERROR_WEAPONMINIGUN_H
#define TRIALANDERROR_WEAPONMINIGUN_H

#include "ProjectileWeapon.h"

class WeaponMinigun : public ProjectileWeapon {
private:
    double m_RecoilForce;

    double m_RateAcceleration;
    double m_RateDeacceleration;
    double m_ShootRate;
    double m_FullRate;

public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    WeaponMinigun(Character* owner);

    void Tick() override;
};

#endif //TRIALANDERROR_WEAPONMINIGUN_H