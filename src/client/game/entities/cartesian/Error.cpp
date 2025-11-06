// Copied Error.cpp into cartesian
#include "Error.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <random>
#include <string>

LoadTexture Error::sTextureErrorDisorianted("entity.error.disoriented", AssetsClass::TexturePurpose::GUI_ELEMENT);
LoadTexture Error::sTextureErrorSpiky("entity.error.cactus", AssetsClass::TexturePurpose::GUI_ELEMENT);
LoadTexture Error::sTextureErrorConfusingHP("entity.error.confused", AssetsClass::TexturePurpose::GUI_ELEMENT);
LoadTexture Error::sTextureErrorInvincible("entity.error.invincible", AssetsClass::TexturePurpose::GUI_ELEMENT);
LoadTexture Error::sTextureErrorHealersParadise("entity.error.healer", AssetsClass::TexturePurpose::GUI_ELEMENT);
LoadTexture Error::sTextureErrorRanged("entity.error.ranged", AssetsClass::TexturePurpose::GUI_ELEMENT);
LoadTexture Error::sTextureErrorSlowDown("entity.error.slow", AssetsClass::TexturePurpose::GUI_ELEMENT);
LoadTexture Error::sTextureErrorDangerousRecoil("entity.error.golden_apple", AssetsClass::TexturePurpose::GUI_ELEMENT);
LoadTexture Error::sTextureMagicParticle("particle.magic", AssetsClass::TexturePurpose::GUI_ELEMENT);
LinkSound Error::ms_PickupSounds[7] = {
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
	LinkSound("entity.error.pickup"),
};

Error::Error(const Vec3f& start_pos, int typeID)
	: Entity(NORMAL_ENTITY,
			 ENTITY_ERROR,
			 start_pos,
			 Vec3f(45, 0, 45),
			 Vec3f(0, 0, 0),
			 0.95,
			 false)
{
	m_Type = static_cast<ErrorType>(typeID);
	if (m_Type == SPIKY) m_Texture = sTextureErrorSpiky.GetTexture();
	else if (m_Type == INVINCIBLE) m_Texture = sTextureErrorInvincible.GetTexture();
	else if (m_Type == SLOW_DOWN) m_Texture = sTextureErrorSlowDown.GetTexture();
	else if (m_Type == HEALERS_PARADISE) m_Texture = sTextureErrorHealersParadise.GetTexture();
	else if (m_Type == DISORIENTED) m_Texture = sTextureErrorDisorianted.GetTexture();
	else if (m_Type == CONFUSING_HP) m_Texture = sTextureErrorConfusingHP.GetTexture();
	else if (m_Type == RANGED) m_Texture = sTextureErrorRanged.GetTexture();
	else m_Texture = sTextureErrorDangerousRecoil.GetTexture();
}

void Error::TickPickup(double x, double y)
{
	auto characters = world->GetEntitiesByType(ENTITY_CHARACTER);
	for (Entity *entity : characters)
	{
		Character *character = (Character *)entity;
		auto& core = character->GetDirectionalCore();
		bool Collides = (core.pos.x - core.size.x / 2 - core.size_ratio < x) &&
			(core.pos.x + core.size.x / 2 + core.size_ratio > x) &&
			(core.pos.y - core.size.y / 2 - core.size_ratio < y) &&
			(core.pos.y + core.size.y / 2 + core.size_ratio > y);

		if (!Collides)
			continue;

		ms_PickupSounds[rand() % 7].GetSound()->PlaySound();

		if (m_Type == DISORIENTED)
		{
			if (character->IsNPC())
			{ character->GetErrorStatuses().Disoriented.Activate(); }
			else
			{
				for (Entity *other_entity : world->GetEntitiesByType(ENTITY_CHARACTER))
				{
					Character *other_character = (Character *)other_entity;
					if (character == other_character) continue;
					if (!other_character->IsNPC()) continue;

					other_character->GetErrorStatuses().Disoriented.Activate();
				}
			}
		}
		else if (m_Type == CONFUSING_HP)
		{
			for (Entity *other_entity : world->GetEntitiesByType(ENTITY_CHARACTER))
			{
				Character *other_character = (Character *)other_entity;
				other_character->GetErrorStatuses().ConfusingHealth.Activate();
			}
		}
		else if (m_Type == INVINCIBLE)
		{
			character->GetErrorStatuses().Invincible.Activate();
		}
		else if (m_Type == SPIKY)
		{
			character->GetErrorStatuses().Spiky.Activate();
		}
		else if (m_Type == HEALERS_PARADISE)
		{
			if (character->IsNPC())
			{
				character->GetErrorStatuses().HealersParadise.Activate();
			}
			else
			{
				for (Entity *other_entity : world->GetEntitiesByType(ENTITY_CHARACTER))
				{
					Character *other_character = (Character *)other_entity;
					if (other_character->IsNPC()) continue;
					other_character->GetErrorStatuses().HealersParadise.Activate();
				}
			}
		}
		else if (m_Type == RANGED)
		{
			character->GetErrorStatuses().RangedFists.Activate();
		}
		else if (m_Type == SLOW_DOWN)
		{
			if (character->IsNPC())
			{
				character->GetErrorStatuses().Slowdown.Activate();
			}
			else
			{
				for (Entity *other_entity : world->GetEntitiesByType(ENTITY_CHARACTER))
				{
					Character *other_character = (Character *)other_entity;
					if (!other_character->IsNPC()) continue;
					other_character->GetErrorStatuses().Slowdown.Activate();
				}
			}
		}
		else if (m_Type == DANGEROUS_RECOIL)
		{
			character->GetErrorStatuses().DangerousRecoil.Activate();
		}

		alive = false;
	}
}

void Error::Tick(double seconds_elapsed)
{
	TickPickup(core.pos.x, core.pos.y);
	TickWalls();

	if (world->GetTick() % (30 + rand() % 30) != 0)
		return;

	world->GetParticles()->PlayParticle(
		Particle(
			sTextureMagicParticle.GetTexture(),
			core.pos,
			Vec3f(0, 5, 0),
			Vec3f((float)(rand() % 10 - 5) / 10.0f, 1.0f, (float)(rand() % 10 - 5) / 10.0f),
			1.0,
			0.0,
			0.0,
			1.0,
			60
		)
	);
}

void Error::Draw()
{
//	DrawingClass *drawing = Application.GetDrawing();
//	SDL_FRect draw_rect = { float(core.pos.x) - float(core.size.x / 2.0),
//							float(core.pos.y) - float(core.size.y / 2.0),
//							float(core.size.x),
//							float(core.size.y) };
//
//	drawing->RenderTexture(m_Texture->SDLTexture(), nullptr, draw_rect, GameReference.GetCamera());
}
