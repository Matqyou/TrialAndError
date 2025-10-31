//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "ProjectileWeapon.h"

class WeaponBurst : public ProjectileWeapon
{
private:
	double m_RecoilForce, m_BaseRecoilForce;
	double m_Damage;

	unsigned long long m_BurstTick;
	int m_BurstCooldown;
	int m_BurstShots;
	int m_BurstShotsLeft;

public:
	static Vec2f sHoldPosition;
	static std::pair<Vec2f, Vec2f> sHandPositions;

	explicit WeaponBurst(Character *owner);

	// Ticking
	void Tick() override;
};
