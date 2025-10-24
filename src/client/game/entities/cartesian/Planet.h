// Copied Planet.h into cartesian
#pragma once

#include "Entity.h"

class Planet : public Entity
{
protected:
    double m_Radius;
    SDL_Color m_Color;
    std::string m_TextureKey;
    Texture *m_Texture{nullptr};
    double m_Rotation = 0.0; // radians

public:
    void SetRotation(double r) { m_Rotation = r; }
    [[nodiscard]] double Rotation() const { return m_Rotation; }

public:
    Planet(GameWorld *world, const Vec2d &pos, double radius, SDL_Color color = {120,120,255,255}, const std::string &textureKey = "");
    ~Planet() override;

    [[nodiscard]] double Radius() const { return m_Radius; }

    void Tick() override;
    void Draw() override;
};