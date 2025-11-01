// Copied Crate.cpp into cartesian
#include "Crate.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include "client/game/entities/cartesian/item/weapons/EntityGuns.h"
#include <iostream>

LinkSound Crate::sHitSound[] = {
	LinkSound("entity.crate.broken.1"),
	LinkSound("entity.crate.broken.2"),
	LinkSound("entity.crate.broken.3"),
};
LinkSound Crate::sBoxSound("entity.crate.hurt");
LinkTexture Crate::sBoxTexture("entity.crate");
LinkTexture Crate::sBreakingBox1Texture("entity.crate2");
LinkTexture Crate::sBreakingBox2Texture("entity.crate3");

Crate::Crate(const Vec2f& start_pos, DropType RandomDrop)
	: Entity(NORMAL_ENTITY,
			 ENTITY_CRATE,
			 start_pos,
			 Vec2f(50, 50),
			 Vec2f(0.0, 0.0),
			 0.95,
			 true,
			 20.0)
{
	world = world;
	alive = true;
	m_DropType = RandomDrop;
	m_Texture = sBoxTexture.GetTexture();

	auto Random = Application.GetRandomizer();
	float RandomFloat = Random->PercentageFloat();
	if (RandomFloat <= 1 / 8.0f) m_ErrorType = DISORIENTED;
	else if (RandomFloat <= 2 / 8.0f)
	{ m_ErrorType = SPIKY; }
	else if (RandomFloat <= 3 / 8.0f)
	{ m_ErrorType = CONFUSING_HP; }
	else if (RandomFloat <= 4 / 8.0f)
	{ m_ErrorType = INVINCIBLE; }
	else if (RandomFloat <= 5 / 8.0f)
	{ m_ErrorType = HEALERS_PARADISE; }
	else if (RandomFloat <= 6 / 8.0f)
	{ m_ErrorType = RANGED; }
	else if (RandomFloat <= 7 / 8.0f)
	{ m_ErrorType = SLOW_DOWN; }
	else if (RandomFloat <= 8 / 8.0f)
	{ m_ErrorType = DANGEROUS_RECOIL; }
}

Crate::~Crate()
{
	auto characters = world->GetEntitiesByType(ENTITY_CHARACTER);
	for (auto entity : characters)
	{
		Character *Char = (Character *)entity;
		Hook *TargetHook = Char->GetHook();
		if (TargetHook->grabbed_entity == this)
			TargetHook->Unhook();
	}
}

void Crate::Damage(double value, Entity *damager)
{
	Sound *BoxHitSound = sBoxSound.GetSound();
	BoxHitSound->PlaySound();
	health_component.ChangeHealthBy(-value);

	if (health_component.m_Health < 10) m_Texture = sBreakingBox2Texture.GetTexture();
	else if (health_component.m_Health < 20) m_Texture = sBreakingBox1Texture.GetTexture();
}

void Crate::Heal(double value)
{
	health_component.ChangeHealthBy(+value);
}

void Crate::Tick(double elapsed_seconds)
{
	TickWalls();

	if (!health_component.IsAlive())
	{
		alive = false;
		sHitSound[rand() % 3].GetSound()->PlaySound();
		if (m_DropType == AMMO)
		{
			auto Ammo_type = Application.GetRandomizer()->UnsignedInt() % NUM_AMMO_TYPES;
			auto new_ammo_box = new AmmoBox(AmmoType(Ammo_type), core.pos, 20);
			world->AddEntity(new_ammo_box, true);
		}
		else if (m_DropType == ERROR)
		{
			auto new_error = new Error(core.pos, m_ErrorType);
			world->AddEntity(new_error, true);
		}
		else if (m_DropType == WEAPON)
		{
			auto weapon_type = static_cast<WeaponType>(rand() % static_cast<int>(NUM_WEAPONS));
			ProjectileWeapon* weapon_data = ProjectileWeapon::CreateWeaponFromWeaponType(weapon_type);
			auto new_item_entity = EntityGuns::CreateItemEntityFromWeaponData(nullptr, weapon_data, core.pos);
			world->AddEntity(new_item_entity, true);
		}
//		else if (m_DropType == WEAPON)
//		{
//			ProjectileWeapon* weapon_data = ProjectileWeapon::CreateWeaponFromWeaponType((WeaponType)(rand() % static_cast<int>(NUM_WEAPONS)));
//			EntityGuns::CreateItemEntityFromWeaponData(m_World, nullptr, weapon_data, m_Core.pos);
//		}
	}
}

void Crate::Draw()
{
	auto drawing = Application.GetDrawing();
	SDL_FRect draw_rect = {
		float(core.pos.x) - float(core.size.x / 2.0),
		float(core.pos.y) - float(core.size.y / 2.0),
		float(core.size.x),
		float(core.size.y)
	};
	drawing->RenderTexture(m_Texture->SDLTexture(), nullptr, draw_rect, GameReference.GetCamera());
}
