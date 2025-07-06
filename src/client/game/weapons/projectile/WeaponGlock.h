//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "ProjectileWeapon.h"
#include "../../entities/Entity.h"

class WeaponGlock : public ProjectileWeapon
{
private:
	double m_RecoilForce;
	double m_Damage;

public:
	static Vec2d sHoldPosition;
	static std::pair<Vec2d, Vec2d> sHandPositions;

	explicit WeaponGlock(DirectionalEntity *parent);

	// Ticking
	void Tick() override;
};
