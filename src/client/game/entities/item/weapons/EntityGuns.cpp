//
// Created by Matq on 04/06/2023.
//

#include "EntityGuns.h"
#include <client/game/entities/cartesian/characters/character/Character.h>

void EntityGlock::EventPickup(Character& picker_char)
{
	picker_char.GiveWeapon(m_Glock);
	m_Glock = nullptr;
	m_Alive = false;
}

EntityGlock::EntityGlock(GameWorld *world, Entity *dropper, WeaponGlock *glock, const Vec2f& start_pos)
	: ItemEntity(world, ITEMTYPE_GLOCK, dropper, start_pos, Vec2f(12 * 4, 8 * 4))
{
	m_Glock = glock ? glock : new WeaponGlock(nullptr);
}

EntityGlock::~EntityGlock()
{
	delete m_Glock;
}

void EntityShotgun::EventPickup(Character& picker_char)
{
	picker_char.GiveWeapon(m_Shotgun);
	m_Shotgun = nullptr;
	m_Alive = false;
}

EntityShotgun::EntityShotgun(GameWorld *world, Entity *dropper, WeaponShotgun *shotgun, const Vec2f& start_pos)
	: ItemEntity(world, ITEMTYPE_SHOTGUN, dropper, start_pos, Vec2f(21 * 4, 6 * 4))
{
	m_Shotgun = shotgun ? shotgun : new WeaponShotgun(nullptr);

}
EntityShotgun::~EntityShotgun()
{
	delete m_Shotgun;
}

void EntityBurst::EventPickup(Character& picker_char)
{
	picker_char.GiveWeapon(m_Burst);
	m_Alive = false;
	m_Burst = nullptr;
}

EntityBurst::EntityBurst(GameWorld *world, Entity *dropper, WeaponBurst *burst, const Vec2f& start_pos)
	: ItemEntity(world, ITEMTYPE_BURST, dropper, start_pos, Vec2f(20 * 4, 8 * 4))
{
	m_Burst = burst ? burst : new WeaponBurst(nullptr);

}
EntityBurst::~EntityBurst()
{
	delete m_Burst;
}

void EntitySniper::EventPickup(Character& picker_char)
{
	picker_char.GiveWeapon(m_Sniper);
	m_Sniper = nullptr;
	m_Alive = false;
}

EntitySniper::EntitySniper(GameWorld *world, Entity *dropper, WeaponSniper *sniper, const Vec2f& start_pos)
	: ItemEntity(world, ITEMTYPE_SNIPER, dropper, start_pos, Vec2f(33 * 4, 9 * 4))
{
	m_Sniper = sniper ? sniper : new WeaponSniper(nullptr);
}

EntitySniper::~EntitySniper()
{
	delete m_Sniper;
}

void EntityMinigun::EventPickup(Character& picker_char)
{
	picker_char.GiveWeapon(m_Minigun);
	m_Minigun->OnSelected();
	m_Alive = false;
	m_Minigun = nullptr;
}

EntityMinigun::EntityMinigun(GameWorld *world, Entity *dropper, WeaponMinigun *minigun, const Vec2f& start_pos)
	: ItemEntity(world, ITEMTYPE_MINIGUN, dropper, start_pos, Vec2f(17 * 4, 8 * 4))
{
	m_Minigun = minigun ? minigun : new WeaponMinigun(nullptr);

}
EntityMinigun::~EntityMinigun()
{
	delete m_Minigun;
}

void EntityPatersonNavy::EventPickup(Character& picker_char)
{
	picker_char.GiveWeapon(m_PatersonNavy);
	m_PatersonNavy = nullptr;
	m_Alive = false;
}

EntityPatersonNavy::EntityPatersonNavy(GameWorld *world,
									   Entity *dropper,
									   PatersonNavy *paterson_navy,
									   const Vec2f& start_pos)
	: ItemEntity(world, ITEMTYPE_PATERSONNAVY, dropper, start_pos, Vec2f(72, 32))
{
	m_PatersonNavy = paterson_navy ? paterson_navy : new PatersonNavy(nullptr);
}

EntityPatersonNavy::~EntityPatersonNavy()
{
	delete m_PatersonNavy;
}