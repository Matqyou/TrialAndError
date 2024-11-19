//
// Created by Matq on 15/05/2023.
//

#include "Hands.h"
#include "Character.h"
#include <cmath>

Texture* Hands::ms_FistTexture = nullptr;

Hands::Hands(Character* parent, double hand_spacing, double fist_animation_duration)
    : m_HandSpacing(hand_spacing / 180.0 * M_PI),
      m_FistingAnimationDuration(fist_animation_duration) {
    m_Parent = parent;

    m_LastFisted = 0;
    m_LastFistedL = 0;
    m_LastFistedR = 0;

    auto& ParentCore = m_Parent->GetDirectionalCore();
    m_BaseFistingRadius = ParentCore.sizeRatio / 2.5;
    m_FistingRadius = ParentCore.sizeRatio / 2.5;
    m_xLeft = ParentCore.Size.x /2.0;
    m_yLeft = ParentCore.Size.y /2.0;
    m_xRight = ParentCore.Size.x /2.0;
    m_yRight = ParentCore.Size.y /2.0;
    m_Size = ParentCore.sizeRatio * 0.36 *2;
    m_Size2 = m_Size / 2.0;
}

Hands::~Hands() {

}

void Hands::SetColor(SDL_Color& color) {
    m_Color = color;
}

void Hands::Tick() {
    if (!m_Parent->Ranged)
        m_FistingRadius = m_BaseFistingRadius;
    else if (m_Parent->Ranged && m_FistingRadius != m_BaseFistingRadius + 100)
        m_FistingRadius = m_BaseFistingRadius + 100;

    if (m_Parent->GetCurrentWeapon())
        return;

    GameWorld* World = m_Parent->World();
    auto CurrentTick = World->GetTick();
    if (CurrentTick - m_LastFisted < 5)
        return;

    if (m_Parent->GetInput().m_Shooting && !m_Parent->GetLastInput().m_Shooting) {
        m_LastFisted = CurrentTick;

        auto& ParentCore = m_Parent->GetDirectionalCore();
        double Radians = m_Parent->GetDirectionalCore().Direction.Atan2();

        double XHands, YHands;
        if (m_LastFistedR < m_LastFistedL) {
            m_LastFistedR = CurrentTick;
            XHands = ParentCore.Pos.x + std::cos(m_HandSpacing + Radians) * m_xRight
                + m_Parent->GetInput().m_LookingX * m_FistingRadius;
            YHands = ParentCore.Pos.y + std::sin(m_HandSpacing + Radians) * m_yRight
                + m_Parent->GetInput().m_LookingY * m_FistingRadius;
        } else {
            m_LastFistedL = CurrentTick;
            XHands = ParentCore.Pos.x + std::cos(-m_HandSpacing + Radians) * m_xLeft
                + m_Parent->GetInput().m_LookingX * m_FistingRadius;
            YHands = ParentCore.Pos.y + std::sin(-m_HandSpacing + Radians) * m_yLeft
                + m_Parent->GetInput().m_LookingY * m_FistingRadius;
        }

        auto Ent = World->FirstEntity();
        for (; Ent; Ent = Ent->Next()) {
            if (Ent == m_Parent) continue;
            if (Ent->GetType() == ENTTYPE_CHARACTER
                && m_Parent->IsNPC() == ((Character*)Ent)->IsNPC())
                continue;

            EntityCore& EntCore = Ent->GetCore();
            double XClosest =
                std::max(EntCore.Pos.x - EntCore.Size.x / 2.0, std::min(EntCore.Pos.x + EntCore.Size.x / 2.0, XHands));
            double YClosest =
                std::max(EntCore.Pos.y - EntCore.Size.y / 2.0, std::min(EntCore.Pos.y + EntCore.Size.y / 2.0, YHands));
            double Distance = std::sqrt(std::pow(XClosest - XHands, 2) + std::pow(YClosest - YHands, 2));
            if (Distance > m_FistingRadius)
                continue;

            if (!m_Parent->IsNPC() && Ent->GetType() == ENTTYPE_CRATE) {
                ((Crate*)Ent)->Damage(5);
                continue;
            }

            if (Ent->GetType() != ENTTYPE_CHARACTER)
                continue;
            Ent->Accelerate(Vec2d(m_Parent->GetInput().m_LookingX, m_Parent->GetInput().m_LookingY) * 5.0);
            ((Character*)Ent)->Damage(7, true);

        }
    }
}

void Hands::Draw() {
    GameWorld* World = m_Parent->World();
    auto& ParentCore = m_Parent->GetDirectionalCore();
    Drawing* Render = World->GameWindow()->Render();

    // TODO: Make different hand positions for different weapons (interesting)
    if (m_Parent->GetCurrentWeapon()) // state
        return;

    auto CurrentTick = World->GetTick();
    double XDirection = m_Parent->GetDirectionalCore().Direction.x;
    double YDirection = m_Parent->GetDirectionalCore().Direction.y;
    double Radians = std::atan2(YDirection, XDirection); // Y, X
    double Angle = Radians / M_PI * 180.0;

    double FistingKoefficientL = double(CurrentTick - m_LastFistedL) / double(m_FistingAnimationDuration);
    double FistingKoefficientR = double(CurrentTick - m_LastFistedR) / double(m_FistingAnimationDuration);
    if (FistingKoefficientL > 1.0) FistingKoefficientL = 1.0;
    if (FistingKoefficientR > 1.0) FistingKoefficientR = 1.0;

    FistingKoefficientL = (1.0 - FistingKoefficientL) * m_FistingRadius;
    FistingKoefficientR = (1.0 - FistingKoefficientR) * m_FistingRadius;

    double XOffLeft = std::cos(-m_HandSpacing + Radians) * m_xLeft + XDirection * FistingKoefficientL;
    double YOffLeft = std::sin(-m_HandSpacing + Radians) * m_yLeft+ YDirection * FistingKoefficientL;
    double XOffRight = std::cos(m_HandSpacing + Radians) * m_xRight + XDirection * FistingKoefficientR;
    double YOffRight = std::sin(m_HandSpacing + Radians) * m_yRight + YDirection * FistingKoefficientR;

    SDL_FRect HandRectLeft = { float(ParentCore.Pos.x - m_Size2 + XOffLeft),
                               float(ParentCore.Pos.y - m_Size2 + YOffLeft),
                               float(m_Size), float(m_Size) };
    SDL_FRect HandRectRight = { float(ParentCore.Pos.x - m_Size2 + XOffRight),
                                float(ParentCore.Pos.y - m_Size2 + YOffRight),
                                float(m_Size), float(m_Size) };

    ms_FistTexture->SetColorMod(m_Color.r, m_Color.g, m_Color.b);
    Render->RenderTextureExFCamera(ms_FistTexture->SDLTexture(), nullptr, HandRectLeft, Angle, nullptr, SDL_FLIP_NONE);
    Render->RenderTextureExFCamera(ms_FistTexture->SDLTexture(), nullptr, HandRectRight, Angle, nullptr, SDL_FLIP_NONE);
}