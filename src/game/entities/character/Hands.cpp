//
// Created by Matq on 15/05/2023.
//

#include "Hands.h"
#include "Character.h"
#include <cmath>

Texture* Hands::ms_FistTexture = nullptr;

Hands::Hands(Character* parent, double hand_spacing, double fist_animation_duration, double fisting_radius)
 : m_HandSpacing(hand_spacing / 180.0 * M_PI),
   m_FistingAnimationDuration(fist_animation_duration) {
    m_Parent = parent;

    m_LastFisted = 0;
    m_LastFistedL = 0;
    m_LastFistedR = 0;

    m_FistingRadius = fisting_radius;

    m_xLeft = m_Parent->GetW() / 2.0;
    m_yLeft = -m_Parent->GetH() / 2.0;
    m_xRight = m_Parent->GetW() / 2.0;
    m_yRight = m_Parent->GetH() / 2.0;
    m_Size = 18.0;
    m_Size2 = m_Size / 2.0;
}

Hands::~Hands() {

}

void Hands::SetColor(SDL_Color& color) {
    m_Color = color;
}

void Hands::Tick() {
    if (m_Parent->GetCurrentWeapon())
        return;

    GameWorld* World = m_Parent->World();
    auto CurrentTick = World->CurrentTick();
    if (CurrentTick - m_LastFisted < 5)
        return;

    if (m_Parent->GetInput().m_Shooting && !m_Parent->GetLastInput().m_Shooting) { // TODO: Fix inputs
        m_LastFisted = CurrentTick;

        double Radians = std::atan2(m_Parent->GetInput().m_LookingY, m_Parent->GetInput().m_LookingX);

        double XHands, YHands;
        if (m_LastFistedR < m_LastFistedL) {
            m_LastFistedR = CurrentTick;
            XHands = m_Parent->GetX() + std::cos(m_HandSpacing + Radians) * 25.0 + m_Parent->GetInput().m_LookingX * m_FistingRadius;
            YHands = m_Parent->GetY() + std::sin(m_HandSpacing + Radians) * 25.0 + m_Parent->GetInput().m_LookingY * m_FistingRadius;
        } else {
            m_LastFistedL = CurrentTick;
            XHands = m_Parent->GetX() + std::cos(-m_HandSpacing + Radians) * 25.0 + m_Parent->GetInput().m_LookingX * m_FistingRadius;
            YHands = m_Parent->GetY() + std::sin(-m_HandSpacing + Radians) * 25.0 + m_Parent->GetInput().m_LookingY * m_FistingRadius;
        }

        auto Ent = World->FirstEntity();
        for (; Ent; Ent = Ent->Next()) {
            if (Ent == m_Parent)
                continue;

            double XClosest = std::max(Ent->GetX() - Ent->GetW() / 2.0, std::min(Ent->GetX() + Ent->GetW() / 2.0, XHands));
            double YClosest = std::max(Ent->GetY() - Ent->GetH() / 2.0, std::min(Ent->GetY() + Ent->GetH() / 2.0, YHands));
            double Distance = std::sqrt(std::pow(XClosest - XHands, 2) + std::pow(YClosest - YHands, 2));
            if (Distance > m_FistingRadius)
                continue;

            Ent->Accelerate(m_Parent->GetInput().m_LookingX * 5.0, m_Parent->GetInput().m_LookingY * 5.0);
            if (Ent->EntityType() != GameWorld::ENTTYPE_CHARACTER)
                continue;

            ((Character*)Ent)->Damage(7, true);
        }
    }
}

void Hands::Draw() {
    GameWorld* World = m_Parent->World();
    Drawing* Render = World->GameWindow()->RenderClass();

    // TODO: Make different hand positions for different weapons (interesting)
    if (m_Parent->GetCurrentWeapon()) // state
        return;

    auto CurrentTick = World->CurrentTick();
    double Radians = std::atan2(m_Parent->GetInput().m_LookingY, m_Parent->GetInput().m_LookingX); // Y, X
    double Angle = Radians / M_PI * 180.0;


    double FistingKoefficientL = double(CurrentTick - m_LastFistedL) / double(m_FistingAnimationDuration);
    double FistingKoefficientR = double(CurrentTick - m_LastFistedR) / double(m_FistingAnimationDuration);
    if (FistingKoefficientL > 1.0) FistingKoefficientL = 1.0;
    if (FistingKoefficientR > 1.0) FistingKoefficientR = 1.0;

    FistingKoefficientL = (1.0 - FistingKoefficientL) * m_FistingRadius;
    FistingKoefficientR = (1.0 - FistingKoefficientR) * m_FistingRadius;

    double XOffLeft = std::cos(-m_HandSpacing + Radians) * 25.0 + m_Parent->GetInput().m_LookingX * FistingKoefficientL;
    double YOffLeft = std::sin(-m_HandSpacing + Radians) * 25.0 + m_Parent->GetInput().m_LookingY * FistingKoefficientL;
    double XOffRight = std::cos(m_HandSpacing + Radians) * 25.0 + m_Parent->GetInput().m_LookingX * FistingKoefficientR;
    double YOffRight = std::sin(m_HandSpacing + Radians) * 25.0 + m_Parent->GetInput().m_LookingY * FistingKoefficientR;

    SDL_FRect HandRectLeft = { float(m_Parent->GetX() - m_Size2 + XOffLeft),
                               float(m_Parent->GetY() - m_Size2 + YOffLeft),
                               float(m_Size), float(m_Size) };
    SDL_FRect HandRectRight = { float(m_Parent->GetX() - m_Size2 + XOffRight),
                                float(m_Parent->GetY() - m_Size2 + YOffRight),
                                float(m_Size), float(m_Size) };

    ms_FistTexture->SetColorMod(m_Color.r, m_Color.g, m_Color.b);
    Render->RenderTextureExFWorld(ms_FistTexture->SDLTexture(), nullptr, HandRectLeft, Angle, nullptr, SDL_FLIP_NONE);
    Render->RenderTextureExFWorld(ms_FistTexture->SDLTexture(), nullptr, HandRectRight, Angle, nullptr, SDL_FLIP_NONE);
}