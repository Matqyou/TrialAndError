//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "client/game/weapons/projectile/base/ProjectileWeapon.h"
#include "client/game/entities/cartesian/base/Entity.h"

class PatersonNavy : public ProjectileWeapon
{
private:
	float m_RecoilForce, m_BaseRecoilForce;
	double m_Damage;

public:
	static Vec2f sHoldPosition;
	static std::pair<Vec2f, Vec2f> sHandPositions;

	explicit PatersonNavy(DirectionalEntity *parent);

	// Ticking
	void Tick() override;
};
