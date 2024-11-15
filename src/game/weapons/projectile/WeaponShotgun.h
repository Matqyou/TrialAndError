//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "ProjectileWeapon.h"

class WeaponShotgun : public ProjectileWeapon {
private:
    int m_PelletCount;
    double m_RecoilForce, m_BaseRecoilForce;
    double m_Damage;

public:
    static Sound* ms_ShootSound;
    static Sound* ms_ClickSound;

    explicit WeaponShotgun(Character* owner);

    // Ticking
    void Tick() override;
};
