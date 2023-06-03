//
// Created by T450s on 01/04/2023.
//

#ifndef TRIALANDERROR_WEAPONMINIGUN_H
#define TRIALANDERROR_WEAPONMINIGUN_H

#include "ProjectileWeapon.h"

class WeaponMinigun : public ProjectileWeapon {
private:
    double m_RecoilForce, m_BaseRecoilForce;

    double m_RateAcceleration;
    double m_RateDeacceleration;
    double m_FireRate;
    double m_MinimumFireRate;
    double m_FullRate;
    double m_Rotation;

public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    explicit WeaponMinigun(Character* owner);

    // Getting
    [[nodiscard]] double Rotation() const { return m_Rotation; }

    // Ticking
    void Tick() override;
};

#endif //TRIALANDERROR_WEAPONMINIGUN_H
