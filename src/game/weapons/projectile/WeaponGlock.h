//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "ProjectileWeapon.h"
#include "../../entities/Entity.h"

class WeaponGlock : public ProjectileWeapon {
private:
    double m_RecoilForce, m_BaseRecoilForce;
    double m_Damage;

public:
    static LoadedSound sShootSound;
    static LoadedSound sClickSound;

    explicit WeaponGlock(DirectionalEntity* parent);

    // Ticking
    void Tick() override;
};
