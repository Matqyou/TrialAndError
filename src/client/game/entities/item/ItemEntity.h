//
// Created by Matq on 03/06/2023.
//

#pragma once

#include <client/game/entities/cartesian/Entity.h>

enum ItemType
{
	ITEMTYPE_GLOCK,
	ITEMTYPE_SHOTGUN,
	ITEMTYPE_BURST,
	ITEMTYPE_SNIPER,
	ITEMTYPE_MINIGUN,
	ITEMTYPE_PATERSONNAVY,
	NUM_ITEMTYPES
};

class ItemEntity : public Entity
{
protected:
	Entity *m_Dropper;
	ItemType m_ItemType;
	Texture *m_Texture;
	double m_Rotation;
	double m_RotationalVelocity;
	double m_RotationalDamping;
	unsigned long long m_DroppedSince;
	unsigned long long m_PickupCooldown;

	// Setting & Initializing
	void SetTexture(ItemType item_type);

	// Listening & Ticking
	virtual void EventPickup(Character& picker_char);
	void TickPickup();

public:
	ItemEntity(GameWorld *world,
			   ItemType item_type,
			   Entity *dropper,
			   const Vec2d& start_pos,
			   const Vec2d& start_size);

	// Setting
	void SetRotation(double rotation);

	// Manipulating
	void AccelerateRotation(double acceleration);

	// Ticking
	void Tick(double elapsed_seconds) override;
	void Draw() override;
};
