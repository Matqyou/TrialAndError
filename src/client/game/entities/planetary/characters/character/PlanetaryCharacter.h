// PlanetaryCharacter.h (moved to entities/planetary)
#pragma once

#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/world/planetary/PlanetaryGameWorld.h>
#include <client/game/world/planetary/PlanetaryUtils.h>
#include <shared/utility/Vec2.h>
#include <client/core/Assets.h>

#include <cmath>
#include <vector>

// Enhanced Character with planetary movement
class PlanetaryCharacter : public Character {
private:
    PlanetaryWorld* m_PlanetaryWorld;
    PlanetaryCoords m_PlanetaryPos;

protected:
    // Override movement processing
    void TickPlanetaryKeyboardControls();
    void TickControls() override;
    void TickCollision();

public:
    PlanetaryCharacter(PlanetaryGameWorld *planetary_world,
                       Player *player,
                       double max_health,
                       const Vec2f& start_pos,
                       const Vec2f& start_vel,
                       bool is_npc);
    // Planetary-specific getters
    PlanetaryWorld* GetPlanetaryWorld() const { return m_PlanetaryWorld; }
    const PlanetaryCoords& GetPlanetaryPos() const { return m_PlanetaryPos; }

    // Planetary-specific setters
    void SetPlanetaryWorld(PlanetaryWorld* world);
    void SetPlanetaryPosition(const PlanetaryCoords& coords);

    // Movement utilities
    Vec2f GetUpDirection() const;
    Vec2f GetRightDirection() const;

    // Override base methods
    void Draw() override;
    void DrawNameplate() override;
    void Tick(double seconds_elapsed) override;
};
