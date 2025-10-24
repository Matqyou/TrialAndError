#include "PlanetaryCharacterNPC.h"
#include <cstdlib>
#include "../../planetary/PlanetaryGameWorld.h"
#include "../../planetary/PlanetaryUtils.h"
#include "../../entities/Planet.h"
#include "shared/utility/Vec2.h"
#include <iostream>

PlanetaryCharacterNPC::PlanetaryCharacterNPC(PlanetaryGameWorld *planetaryWorld, const Vec2d &pos)
    : CharacterNPC((GameWorld *)planetaryWorld, 100.0, pos, Vec2d(0,0), NPC_DUMMY, false)
{
    if (planetaryWorld && planetaryWorld->GetCurrentPlanet()) {
        m_PlanetaryWorld = planetaryWorld->GetCurrentPlanet();
        m_PlanetaryPos = m_PlanetaryWorld->WorldToPlanetary(pos);
        std::cout << "[PlanetaryNPC] Spawned at lon=" << m_PlanetaryPos.longitude << " lat=" << m_PlanetaryPos.latitude << "\n";
    }
}

void PlanetaryCharacterNPC::Tick()
{
    if (!m_PlanetaryWorld) {
        Character::Tick();
        return;
    }

    PlanetaryCoords pc = m_PlanetaryPos;
    double drift = 0.0008;
    double r1 = (double)(rand() % 1000) / 1000.0;
    double r2 = (double)(rand() % 1000) / 1000.0;
    pc.longitude += (r1 - 0.5) * drift;
    pc.latitude += (r2 - 0.5) * drift;
    pc.NormalizeAngle();

    m_PlanetaryPos = pc;
    // sync core pos so base logic has a sensible pos for rendering/collision
    m_Core.Pos = m_PlanetaryPos.ToCartesian();

    // call Character::Tick to process health, weapons, etc.
    Character::Tick();
}
