//
// Created by T450s on 01/04/2023.
//

#pragma once

#include "ProjectileWeapon.h"

class WeaponShotgun : public ProjectileWeapon
{
private:
	int m_PelletCount;
	double m_RecoilForce, m_BaseRecoilForce;
	double m_Damage;

public:
	static LoadedTexture sTextureWeapon;
	static LoadedTexture sTextureProjectile;
	static LoadedSound sShootSound;
	static LoadedSound sClickSound;
	static LoadedSound sReloadSound;
	static Vec2d sHoldPosition;
	static std::pair<Vec2d, Vec2d> sHandPositions;

	explicit WeaponShotgun(Character *owner);

	// Ticking
	void Tick() override;
};
