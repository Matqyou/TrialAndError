//
// Created by Matq on 15/05/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_CHARACTER_HANDS_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_CHARACTER_HANDS_H_

#include "../../../technical stuff/Colors.h"
#include "../../../technical stuff/ImageManager.h"

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

public:
    static Texture* ms_FistTexture;

    Hands(Character* parent, double hand_spacing, double fist_animation_duration, double fisting_radius);
    ~Hands();

    // Getting
    [[nodiscard]] Character* Parent() const { return m_Parent; }

    // Setting
    void SetColor(SDL_Color& color);

    // Ticking
    void Tick();
    void Draw();
};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_CHARACTER_HANDS_H_
