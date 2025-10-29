//
// Created by Mim on 5/08/2023.
//

#pragma once

#include "Entity.h"
#include <SDL3/SDL.h>
#include "AmmoBox.h"

enum DropType
{
	AMMO,
	ERROR,
	NUM_DROPS
};

class Crate : public Entity
{
protected:
	friend class GameWorld;
	DropType m_DropType;
	int m_ErrorType;
	Texture *m_Texture;

public:
	Crate(GameWorld *world,
		  const Vec2d& start_pos,
		  DropType RandomDrop);
	~Crate() override;

	// Getting
	[[nodiscard]] DropType GetDropType() const { return m_DropType; }

	// Manipulating
	void Damage(double value, Entity *damager);
	void Heal(double value);

	// Ticking
	void Tick(double elapsed_seconds) override;
	void Draw() override;
};
