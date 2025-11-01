//
// Created by Matq on 03/06/2023.
//

#include "client/game/entities/cartesian/item/ItemEntity.h"
#include "client/game/entities/cartesian/characters/character/Character.h"

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
		default:
			break;
	}
}

ItemEntity::ItemEntity(ItemType item_type,
					   Entity *dropper,
					   const Vec2f& start_pos,
					   const Vec2f& start_size)
	: Entity(NORMAL_ENTITY,
			 ENTITY_ITEM,
			 start_pos,
			 start_size,
			 Vec2f(0.0, 0.0),
			 0.95,
			 false)
{
	m_Dropper = dropper;
	m_DroppedSince = 0;
	m_PickupCooldown = (unsigned long long)(Application.GetClock()->GetFramerate());
	m_ItemType = item_type;
	m_Texture = nullptr;
	//m_PickupRadius = 25.0;
	m_Rotation = 0.0;
	m_RotationalVelocity = 0;
	m_RotationalDamping = 0.95;

	// runs when world->AddEntity(...) is called
	entity_added_event.Subscribe([this](GameWorld* our_world) {
		m_DroppedSince = our_world->GetTick();
	});

	SetTexture(item_type);
}

void ItemEntity::EventPickup(Character& picker_char)
{
	alive = false;
}

void ItemEntity::TickPickup()
{
	if (!alive)
		return;

	for (Entity* entity : world->GetEntitiesByType(ENTITY_CHARACTER))
	{
		auto character = (Character*)entity;
		// The code below makes me think what if the m_Dropper entity was dead a long time ago and now
		// a new entity has been summoned with the exact same address... HMMMMMMMM
		// In a world where the pickup cooldown is infinite this entity is doomed to suffer
		// HE will never be able to pick up the item he always wanted........
		// Eh theres probably a way for that entity to reincarnate, but if there isn't there's nothing
		// really he can do about this situation
		// Todo: think of some connected/smart pointers cuz that is the next big thing i need to learn
		if (!character->IsAlive() || (character == m_Dropper && world->GetTick() - m_DroppedSince < m_PickupCooldown)) continue;
		float Distance = DistanceVec2f(core.pos, character->GetCore().pos);
		if (Distance > core.size_ratio + character->GetCore().size_ratio) continue;

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
	SDL_FRect DrawRect = {float(core.pos.x - core.size.x / 2.0),
						  float(core.pos.y - core.size.y / 2.0),
						  float(core.size.x),
						  float(core.size.y) };

	drawing->RenderTextureRotated(m_Texture->SDLTexture(),
								  nullptr, DrawRect,
								  m_Rotation / M_PI * 180.0, nullptr,
								  SDL_FLIP_NONE, GameReference.GetCamera());
}
