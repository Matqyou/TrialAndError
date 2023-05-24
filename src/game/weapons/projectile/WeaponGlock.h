//
// Created by T450s on 01/04/2023.
//

#ifndef TRIALANDERROR_WEAPONGLOCK_H
#define TRIALANDERROR_WEAPONGLOCK_H

#include "ProjectileWeapon.h"

class WeaponGlock : public ProjectileWeapon {
private:
    double m_RecoilForce, m_BaseRecoilForce;
    double m_Damage;

public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    WeaponGlock(Character* owner);

    void Tick() override;
};

#endif //TRIALANDERROR_WEAPONGLOCK_H
