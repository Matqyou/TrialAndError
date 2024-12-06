//
// Created by Matq on 15/05/2023.
//

#pragma once

#include "../../../../technical stuff/Colors.h"
#include "../../../../client/Assets.h"

class Character;
class Hands {
private:
    Character* m_Parent;
    SDL_Color m_Color;
    unsigned long long m_LastFisted;
    unsigned long long m_LastFistedL, m_LastFistedR;
    const double m_HandSpacing;
    const double m_FistingAnimationDuration;
    double m_FistingRadius, m_BaseFistingRadius;

    double m_xLeft, m_yLeft;
    double m_xRight, m_yRight;
    double m_Size, m_Size2;

    static LoadedTexture sFistTexture;

public:
    Hands(Character* parent, double hand_spacing, double fist_animation_duration);
    ~Hands();

    // Getting
    [[nodiscard]] Character* Parent() const { return m_Parent; }
    [[nodiscard]] unsigned long long LastFisted() const { return m_LastFisted; }

    // Setting
    void SetColor(SDL_Color& color);

    // Ticking
    void Tick();
    void Draw();
};
