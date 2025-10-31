//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "ProjectileWeapon.h"
#include <client/game/entities/cartesian/Entity.h>

class WeaponGlock : public ProjectileWeapon
{
private:
	double m_RecoilForce;
	double m_Damage;

public:
	static Vec2f sHoldPosition;
	static std::pair<Vec2f, Vec2f> sHandPositions;

	explicit WeaponGlock(DirectionalEntity *parent);

	// Ticking
	void Tick() override;
};
