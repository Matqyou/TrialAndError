//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "client/game/weapons/projectile/base/ProjectileWeapon.h"

class WeaponShotgun : public ProjectileWeapon
{
private:
	int m_PelletCount;
	float m_RecoilForce, m_BaseRecoilForce;
	double m_Damage;

public:
	static Vec2f sHoldPosition;
	static std::pair<Vec2f, Vec2f> sHandPositions;

	explicit WeaponShotgun(Character *owner);

	// Ticking
	void Tick() override;
};
