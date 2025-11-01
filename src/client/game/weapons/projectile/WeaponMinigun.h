//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "client/game/weapons/projectile/base/ProjectileWeapon.h"

class WeaponMinigun : public ProjectileWeapon
{
private:
	float m_RecoilForce, m_BaseRecoilForce;
	double m_Damage;

	double m_RateAcceleration;
	double m_RateDeacceleration;
	double m_FireRate;
	double m_MinimumFireRate;
	double m_FullRate;
	double m_Rotation;

public:
	static Vec2f sHoldPosition;
	static std::pair<Vec2f, Vec2f> sHandPositions;

	explicit WeaponMinigun(Character *owner);

	// Getting
	[[nodiscard]] double Rotation() const { return m_Rotation; }

	// Ticking
	void Tick() override;
	void Reload() override;
	void OnSelected() override;

};
