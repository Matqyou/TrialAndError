//
// Created by T450s on 01/04/2023.
//

#ifndef TRIALANDERROR_WEAPONGLOCK_H
#define TRIALANDERROR_WEAPONGLOCK_H

#include "ProjectileWeapon.h"
#include "../../entities/Entity.h"

class WeaponGlock : public ProjectileWeapon {
private:
    double m_RecoilForce, m_BaseRecoilForce;
    double m_Damage;

public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    explicit WeaponGlock(DirectionalEntity* parent);

    // Ticking
    void Tick() override;
};

#endif //TRIALANDERROR_WEAPONGLOCK_H
