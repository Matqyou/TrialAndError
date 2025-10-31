//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "ProjectileWeapon.h"
#include <client/game/entities/cartesian/Entity.h>

class WeaponSniper : public ProjectileWeapon
{
private:
	double m_RecoilForce, m_BaseRecoilForce;
	double m_Damage;

public:
	static Vec2f sHoldPosition;
	static std::pair<Vec2f, Vec2f> sHandPositions;

	explicit WeaponSniper(DirectionalEntity *parent);

	// Ticking
	void Tick() override;
};
