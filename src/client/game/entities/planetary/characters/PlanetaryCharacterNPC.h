#pragma once

#include <client/game/entities/cartesian/characters/CharacterNPC.h>
#include <client/game/world/planetary/PlanetaryUtils.h>
#include <client/game/world/planetary/PlanetaryGameWorld.h>

// Planetary NPC: NPC behavior implemented in planetary coordinates
class PlanetaryCharacterNPC : public CharacterNPC
{
protected:
    PlanetaryWorld *m_PlanetaryWorld{nullptr};
    PlanetaryCoords m_PlanetaryPos;

public:
    explicit PlanetaryCharacterNPC(const Vec2f &pos);
    ~PlanetaryCharacterNPC() override = default;

	// Getting
	[[nodiscard]] const PlanetaryCoords &GetPlanetaryPos() const { return m_PlanetaryPos; }

	// Setting
	void SetPlanetaryWorld(PlanetaryWorld *world) { m_PlanetaryWorld = world; }
	void SetPlanetaryPosition(const PlanetaryCoords &coords) { m_PlanetaryPos = coords; }

	// Ticking
    void Tick(double seconds_elapsed) override;

};
