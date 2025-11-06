//
// Created by Matq on 04/06/2023.
//

#pragma once

#include "client/game/weapons/projectile/WeaponShotgun.h"
#include "client/game/weapons/projectile/WeaponMinigun.h"
#include "client/game/weapons/projectile/WeaponSniper.h"
#include "client/game/weapons/projectile/PatersonNavy.h"
#include "client/game/weapons/projectile/WeaponBurst.h"
#include "client/game/weapons/projectile/WeaponGlock.h"
#include "client/game/entities/cartesian/item/ItemEntity.h"

namespace EntityGuns
{
ItemEntity *CreateItemEntityFromWeaponData(Entity *dropper, ProjectileWeapon *weapon_data, const Vec3f& start_pos);

}

class EntityGlock : public ItemEntity
{
private:
	WeaponGlock *m_Glock;

	void EventPickup(Character& picker_char) override;

public:
	EntityGlock(Entity *dropper, WeaponGlock *glock, const Vec3f& start_pos);
	~EntityGlock() override;

};

class EntityShotgun : public ItemEntity
{
private:
	WeaponShotgun *m_Shotgun;
	void EventPickup(Character& picker_char) override;

public:
	EntityShotgun(Entity *dropper, WeaponShotgun *shotgun, const Vec3f& start_pos);
	~EntityShotgun() override;
};

class EntityBurst : public ItemEntity
{
private:
	WeaponBurst *m_Burst;
	// Listening
	void EventPickup(Character& picker_char) override;

public:
	EntityBurst(Entity *dropper, WeaponBurst *burst, const Vec3f& start_pos);
	~EntityBurst() override;
};

class EntitySniper : public ItemEntity
{
private:
	WeaponSniper *m_Sniper;
	// Listening
	void EventPickup(Character& picker_char) override;

public:
	EntitySniper(Entity *dropper, WeaponSniper *sniper, const Vec3f& start_pos);
	~EntitySniper() override;
};

class EntityMinigun : public ItemEntity
{
private:
	WeaponMinigun *m_Minigun;
	// Listening
	void EventPickup(Character& picker_char) override;

public:
	EntityMinigun(Entity *dropper, WeaponMinigun *minigun, const Vec3f& start_pos);
	~EntityMinigun() override;
};

class EntityPatersonNavy : public ItemEntity
{
private:
	PatersonNavy *m_PatersonNavy;

	void EventPickup(Character& picker_char) override;

public:
	EntityPatersonNavy(Entity *dropper, PatersonNavy *paterson_navy, const Vec3f& start_pos);
	~EntityPatersonNavy() override;

};