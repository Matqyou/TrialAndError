// CartesianGameWorld.h
#pragma once

#include <client/game/world/GameWorld.h>
#include <shared/utility/Vec2.h>
#include <client/game/entities/cartesian/Planet.h>
#include <vector>
#include <client/GameData.h>
#include <shared/utility/Vec2.h>
class PlanetEnterMenu;

class CartesianGameWorld : public GameWorld
{
public:
    CartesianGameWorld(GameData *game_window, int width, int height);
    ~CartesianGameWorld() override;

    void Tick() override;
    void Draw() override;

private:
    struct PlanetInfo { Vec2d pos; double radius; SDL_Color color; };
    std::vector<PlanetInfo> m_Planets;

    // Spawned entity pointers
    std::vector<Planet*> m_PlanetEntities;

    // Planet enter menu (interface element)
    PlanetEnterMenu *m_PlanetEnterMenu = nullptr;
};
