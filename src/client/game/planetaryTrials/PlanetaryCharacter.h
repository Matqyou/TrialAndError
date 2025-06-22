// PlanetarySystem.h
#pragma once

#include <cmath>
#include <vector>
#include "SDL.h"
#include "shared/utility/Vec2.h"
#include "client/core/Assets.h"
#include "PlanetaryGameWorld.h"
#include "../entities/characters/character/Character.h"

// Forward declarations
class GameWorld;
class GameData;

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
                      const Vec2d& start_pos,
                      const Vec2d& start_vel,
                      bool is_npc);
    // Planetary-specific getters
    PlanetaryWorld* GetPlanetaryWorld() const { return m_PlanetaryWorld; }
    const PlanetaryCoords& GetPlanetaryPos() const { return m_PlanetaryPos; }
    
    // Planetary-specific setters
    void SetPlanetaryWorld(PlanetaryWorld* world);
    void SetPlanetaryPosition(const PlanetaryCoords& coords);
    
    // Movement utilities
    Vec2d GetUpDirection() const;
    Vec2d GetRightDirection() const;

    // Override base methods
    void Draw() override;
};