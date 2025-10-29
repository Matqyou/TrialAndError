//
// Created by Matq on 03/06/2023.
//

#include "ItemEntity.h"
#include "../characters/character/Character.h"

static LinkTexture sTextureGlock("entity.items.glock");
static LinkTexture sTextureShotgun("entity.items.shotgun");
static LinkTexture sTextureBurst("entity.items.burst");
static LinkTexture sTextureSniper("entity.items.sniper");
static LinkTexture sTexturePatersonNavy("entity.items.paterson_navy");
static LinkTexture sTexturesMinigun[4] = {
	LinkTexture("entity.items.minigun1"),
	LinkTexture("entity.items.minigun2"),
	LinkTexture("entity.items.minigun3"),
	LinkTexture("entity.items.minigun4"),
};

void ItemEntity::SetTexture(ItemType item_type)
{
	switch (item_type)
	{
		case ITEMTYPE_GLOCK:
		{
			m_Texture = sTextureGlock.GetTexture();
			break;
		}
		case ITEMTYPE_SHOTGUN:
		{
			m_Texture = sTextureShotgun.GetTexture();
			break;
		}
		case ITEMTYPE_BURST:
		{
			m_Texture = sTextureBurst.GetTexture();
			break;
		}
		case ITEMTYPE_SNIPER:
		{
			m_Texture = sTextureSniper.GetTexture();
			break;
		}
		case ITEMTYPE_MINIGUN:
		{
			m_Texture = sTexturesMinigun[0].GetTexture(); // TODO depend on rotation
			break;
		}
		case ITEMTYPE_PATERSONNAVY:
		{
			m_Texture = sTexturePatersonNavy.GetTexture();
			break;
		}
	}
}

ItemEntity::ItemEntity(GameWorld *world,
					   ItemType item_type,
					   Entity *dropper,
					   const Vec2d& start_pos,
					   const Vec2d& start_size)
	: Entity(world,
			 NORMAL_ENTITY,
			 ITEM_ENTITY,
			 start_pos,
			 start_size,
			 Vec2d(0.0, 0.0),
			 0.95,
			 false)
{
	m_Dropper = dropper;
	m_DroppedSince = m_World->GetTick();
	m_PickupCooldown = (unsigned long long)(Application.GetClock()->GetFramerate());
	m_ItemType = item_type;
	m_Texture = nullptr;
	//m_PickupRadius = 25.0;
	m_Rotation = 0.0;
	m_RotationalVelocity = 0;
	m_RotationalDamping = 0.95;

	SetTexture(item_type);
}

void ItemEntity::EventPickup(Character& picker_char)
{
	m_Alive = false;
}

void ItemEntity::TickPickup()
{
	if (!m_Alive)
		return;

	for (Entity* entity : m_World->GetEntitiesByType(CHARACTER_ENTITY))
	{
		auto character = (Character*)entity;
		// The code below makes me think what if the m_Dropper entity was dead a long time ago and now
		// a new entity has been summoned with the exact same address... HMMMMMMMM
		// In a world where the pickup cooldown is infinite this entity is doomed to suffer
		// HE will never be able to pick up the item he always wanted........
		// Eh theres probably a way for that entity to reincarnate, but if there isn't there's nothing
		// really he can do about this situation
		// Todo: think of some connected/smart pointers cuz that is the next big thing i need to learn
		if (!character->IsAlive() || (character == m_Dropper && m_World->GetTick() - m_DroppedSince < m_PickupCooldown)) continue;
		double Distance = DistanceVec2d(m_Core.Pos, character->GetCore().Pos);
		if (Distance > m_Core.sizeRatio + character->GetCore().sizeRatio) continue;

		EventPickup(*character);
		break;
	}
}

void ItemEntity::SetRotation(double rotation)
{
	m_Rotation = rotation;
}

void ItemEntity::AccelerateRotation(double acceleration)
{
	m_RotationalVelocity += acceleration;
}

void ItemEntity::Tick(double elapsed_seconds)
{
	TickPickup();
	TickVelocity(elapsed_seconds);
	m_Rotation += m_RotationalVelocity;
	m_RotationalVelocity *= m_RotationalDamping;
}

void ItemEntity::Draw()
{
	if (m_Texture == nullptr)
		return;

	auto drawing = Application.GetDrawing();
	SDL_FRect DrawRect = {float(m_Core.Pos.x - m_Core.Size.x / 2.0),
						  float(m_Core.Pos.y - m_Core.Size.y / 2.0),
						  float(m_Core.Size.x),
						  float(m_Core.Size.y) };

	drawing->RenderTextureRotated(m_Texture->SDLTexture(),
								  nullptr, DrawRect,
								  m_Rotation / M_PI * 180.0, nullptr,
								  SDL_FLIP_NONE, GameReference.GetCamera());
}
