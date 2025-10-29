//
// Created by Mim on 5/13/2023.
//

#include "Error.h"
#include "characters/character/Character.h"
#include <random>
#include <string>

LinkTexture sTextureErrorDisorianted("entity.error.disoriented");
LinkTexture sTextureErrorSpiky("entity.error.cactus");
LinkTexture sTextureErrorConfusingHP("entity.error.confused");
LinkTexture sTextureErrorInvincible("entity.error.invincible");
LinkTexture sTextureErrorHealersParadise("entity.error.healer");
LinkTexture sTextureErrorRanged("entity.error.ranged");
LinkTexture sTextureErrorSlowDown("entity.error.slow");
LinkTexture sTextureErrorDangerousRecoil("entity.error.golden_apple");
LinkTexture sTextureMagicParticle("particle.magic");
LinkSound ms_PickupSounds[7] = {
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
};

Error::Error(GameWorld *world, const Vec2d& start_pos, int typeID)
	: Entity(world,
			 NORMAL_ENTITY,
			 ERROR_ENTITY,
			 start_pos,
			 Vec2d(45, 45),
			 Vec2d(0.0, 0.0),
			 0.95,
			 false)
{

	m_Type = static_cast<ErrorType>(typeID);
	if (m_Type == SPIKY) m_Texture = sTextureErrorSpiky.GetTexture();
	else if (m_Type == INVINCIBLE) m_Texture = sTextureErrorInvincible.GetTexture();
	else if (m_Type == SLOW_DOWN) m_Texture = sTextureErrorSlowDown.GetTexture();
	else if (m_Type == HEALERS_PARADISE) m_Texture = sTextureErrorHealersParadise.GetTexture();
	else if (m_Type == DISORIANTED) m_Texture = sTextureErrorDisorianted.GetTexture();
	else if (m_Type == CONFUSING_HP) m_Texture = sTextureErrorConfusingHP.GetTexture();
	else if (m_Type == RANGED) m_Texture = sTextureErrorRanged.GetTexture();
	else m_Texture = sTextureErrorDangerousRecoil.GetTexture();
}

void Error::TickPickup(double x, double y)
{
	for (Entity* entity : m_World->GetEntitiesByType(CHARACTER_ENTITY))
	{
		auto character = (Character*)entity;
		auto& CharCore = character->GetDirectionalCore();
		bool Collides = (CharCore.Pos.x - CharCore.Size.x / 2 - m_Core.sizeRatio < x) &&
			(CharCore.Pos.x + CharCore.Size.x / 2 + m_Core.sizeRatio > x) &&
			(CharCore.Pos.y - CharCore.Size.y / 2 - m_Core.sizeRatio < y) &&
			(CharCore.Pos.y + CharCore.Size.y / 2 + m_Core.sizeRatio > y);

		if (!Collides)
			continue;

		ms_PickupSounds[rand() % 7].GetSound()->PlaySound();

		if (m_Type == DISORIANTED)
		{
			if (character->IsNPC())
			{ character->GetErrorStatuses().Disoriented.Activate(); }
			else
			{
				for (Entity* entity2 : m_World->GetEntitiesByType(CHARACTER_ENTITY))
				{
					auto character2 = (Character*)entity2;
					if (!character2->IsNPC()) continue;
					character2->GetErrorStatuses().Disoriented.Activate();
				}
			}
		}
		else if (m_Type == CONFUSING_HP)
		{
			for (Entity* entity2 : m_World->GetEntitiesByType(CHARACTER_ENTITY))
			{
				auto character2 = (Character *)entity2;
				character2->GetErrorStatuses().ConfusingHealth.Activate();
			}
		}
		else if (m_Type == INVINCIBLE)
		{ character->GetErrorStatuses().Invincible.Activate(); }
		else if (m_Type == SPIKY)
		{ character->GetErrorStatuses().Spiky.Activate(); }
		else if (m_Type == HEALERS_PARADISE)
		{
			if (character->IsNPC())
			{ character->GetErrorStatuses().HealersParadise.Activate(); }
			else
			{
				for (Entity* entity2 : m_World->GetEntitiesByType(CHARACTER_ENTITY))
				{
					auto character2 = (Character *)entity2;
					if (character2->IsNPC()) continue;
					character2->GetErrorStatuses().HealersParadise.Activate();
				}
			}
		}
		else if (m_Type == RANGED)
		{ character->GetErrorStatuses().RangedFists.Activate(); }
		else if (m_Type == SLOW_DOWN)
		{
			if (character->IsNPC())
			{ character->GetErrorStatuses().Slowdown.Activate(); }
			else
			{
				for (Entity* entity2 : m_World->GetEntitiesByType(CHARACTER_ENTITY))
				{
					auto character2 = (Character *)entity2;
					if (!character2->IsNPC()) continue;
					character2->GetErrorStatuses().Slowdown.Activate();
				}
			}
		}
		else if (m_Type == DANGEROUS_RECOIL)
		{ character->GetErrorStatuses().DangerousRecoil.Activate(); }
		m_Alive = false;
	}
}

void Error::Tick(double elapsed_seconds)
{
	TickPickup(m_Core.Pos.x, m_Core.Pos.y);
	TickWalls();

	if (m_World->GetTick() % (30 + rand() % 30) != 0)
		return;

	m_World->GetParticles()->PlayParticle(Particle(sTextureMagicParticle.GetTexture(),
												   m_Core.Pos,
												   Vec2d(5.0, 5.0),
												   Vec2d((double)(rand() % 10 - 5) / 10.0, -1.0),
												   1.0,
												   0.0,
												   0.0,
												   1.0,
												   60));
}

void Error::Draw()
{
	auto drawing = Application.GetDrawing();
	SDL_FRect DrawRect = { float(m_Core.Pos.x) - float(m_Core.Size.x / 2.0),
						   float(m_Core.Pos.y) - float(m_Core.Size.y / 2.0),
						   float(m_Core.Size.x),
						   float(m_Core.Size.y) };

	drawing->RenderTexture(m_Texture->SDLTexture(), nullptr, DrawRect, GameReference.GetCamera());

}