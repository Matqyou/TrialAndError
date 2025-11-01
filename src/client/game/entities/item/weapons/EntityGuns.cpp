//
// Created by Matq on 04/06/2023.
//

#include "EntityGuns.h"
#include <client/game/entities/cartesian/characters/character/Character.h>

namespace EntityGuns
{
ItemEntity *CreateItemEntityFromWeaponData(Entity *dropper, ProjectileWeapon *weapon_data, const Vec2f& start_pos)
{
	WeaponType weapon_type = weapon_data->GetWeaponType();
	ItemEntity *new_weapon_entity = nullptr;
	switch (weapon_type)
	{
		case WEAPON_GLOCK:
		{
			new_weapon_entity = new EntityGlock(dropper, (WeaponGlock *)weapon_data, start_pos);
//			m_Weapons[WEAPON_GLOCK] = nullptr;
			break;
		}
		case WEAPON_BURST:
		{
			new_weapon_entity = new EntityBurst(dropper, (WeaponBurst *)weapon_data, start_pos);
//			m_Weapons[weapon_type] = nullptr;
			break;
		}
		case WEAPON_SHOTGUN:
		{
			new_weapon_entity = new EntityShotgun(dropper, (WeaponShotgun *)weapon_data, start_pos);
//			m_Weapons[weapon_type] = nullptr;
			break;
		}
		case WEAPON_MINIGUN:
		{
			new_weapon_entity = new EntityMinigun(dropper, (WeaponMinigun *)weapon_data, start_pos);
//			m_Weapons[weapon_type] = nullptr;
			break;
		}
		case WEAPON_SNIPER:
		{
			new_weapon_entity = new EntitySniper(dropper, (WeaponSniper *)weapon_data, start_pos);
//			m_Weapons[weapon_type] = nullptr;
			break;
		}
		case WEAPON_PATERSONNAVY:
		{
			new_weapon_entity = new EntityPatersonNavy(dropper, (PatersonNavy *)weapon_data, start_pos);
//			m_Weapons[weapon_type] = nullptr;
			break;
		}
		default: break;
	}

//	new_weapon_entity->Accelerate(m_DirectionalCore.direction * 20);
//	new_weapon_entity->SetRotation(m_DirectionalCore.direction.Atan2F());
//	new_weapon_entity->AccelerateRotation(std::fmod(Application.GetRandomizer()->Float(), 0.7f) - 0.35f);
//	m_CurrentWeapon = nullptr;
	return new_weapon_entity;
}
}

void EntityGlock::EventPickup(Character& picker_char)
{
	picker_char.GiveWeapon(m_Glock);
	m_Glock = nullptr;
	alive = false;
}

EntityGlock::EntityGlock(Entity *dropper, WeaponGlock *glock, const Vec2f& start_pos)
	: ItemEntity(ITEMTYPE_GLOCK, dropper, start_pos, Vec2f(12 * 4, 8 * 4))
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
	alive = false;
}

EntityShotgun::EntityShotgun(Entity *dropper, WeaponShotgun *shotgun, const Vec2f& start_pos)
	: ItemEntity(ITEMTYPE_SHOTGUN, dropper, start_pos, Vec2f(21 * 4, 6 * 4))
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
	alive = false;
	m_Burst = nullptr;
}

EntityBurst::EntityBurst(Entity *dropper, WeaponBurst *burst, const Vec2f& start_pos)
	: ItemEntity(ITEMTYPE_BURST, dropper, start_pos, Vec2f(20 * 4, 8 * 4))
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
	alive = false;
}

EntitySniper::EntitySniper(Entity *dropper, WeaponSniper *sniper, const Vec2f& start_pos)
	: ItemEntity(ITEMTYPE_SNIPER, dropper, start_pos, Vec2f(33 * 4, 9 * 4))
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
	alive = false;
	m_Minigun = nullptr;
}

EntityMinigun::EntityMinigun(Entity *dropper, WeaponMinigun *minigun, const Vec2f& start_pos)
	: ItemEntity(ITEMTYPE_MINIGUN, dropper, start_pos, Vec2f(17 * 4, 8 * 4))
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
	alive = false;
}

EntityPatersonNavy::EntityPatersonNavy(Entity *dropper,
									   PatersonNavy *paterson_navy,
									   const Vec2f& start_pos)
	: ItemEntity(ITEMTYPE_PATERSONNAVY, dropper, start_pos, Vec2f(72, 32))
{
	m_PatersonNavy = paterson_navy ? paterson_navy : new PatersonNavy(nullptr);
}

EntityPatersonNavy::~EntityPatersonNavy()
{
	delete m_PatersonNavy;
}