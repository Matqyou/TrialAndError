// Copied Planet.h into cartesian
#pragma once

#include "Entity.h"

class Planet : public Entity
{
protected:
    double m_Radius;
    SDL_Color m_Color;
    std::string m_TexturePath;
    Texture *m_Texture;

public:
    Planet(GameWorld *world, const Vec2d& pos, double radius, SDL_Color color, const std::string& texture_path);
    ~Planet() override;

    double Radius() const { return m_Radius; }
    void Draw() override;
};
