#pragma once

#include "../characters/CharacterNPC.h"
#include "../../planetary/PlanetaryUtils.h"
#include "../../planetary/PlanetaryGameWorld.h"

// Planetary NPC: NPC behavior implemented in planetary coordinates
class PlanetaryCharacterNPC : public CharacterNPC
{
protected:
    PlanetaryWorld *m_PlanetaryWorld{nullptr};
    PlanetaryCoords m_PlanetaryPos;

public:
    PlanetaryCharacterNPC(PlanetaryGameWorld *planetaryWorld, const Vec2d &pos);
    virtual ~PlanetaryCharacterNPC() override = default;

    void SetPlanetaryWorld(PlanetaryWorld *world) { m_PlanetaryWorld = world; }
    void SetPlanetaryPosition(const PlanetaryCoords &coords) { m_PlanetaryPos = coords; }
    const PlanetaryCoords &GetPlanetaryPos() const { return m_PlanetaryPos; }

    void Tick() override;
};
