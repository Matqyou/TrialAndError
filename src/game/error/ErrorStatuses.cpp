//
// Created by Matq on 13/12/2024.
//

#include "ErrorStatuses.h"

LoadedTexture ErrorStatuses::sTextureErrorFrame("interface.ingame.error_frame");
LoadedTexture ErrorStatuses::sTextureErrorBackground("interface.ingame.error_background");

ErrorStatuses::ErrorStatuses(GameWorld* world, Character* parent)
 : BulletFrenzy(this),
      CantHear(this),
      ConfusingHealth(this),
      DangerousRecoil(this),
      Disoriented(this),
      HealersParadise(this),
      Invincible(this),
      RangedFists(this),
      Slowdown(this),
      Spiky(this),
      Teleport(this) {
    m_World = world;
    m_Parent = parent;

    m_Drawing = world->GameWindow()->Render();

    m_Frames.CreateFrames(m_Effects);
}

bool ErrorStatuses::AnyActive(double from_seconds_ago) {
    auto ticks_ago = (unsigned long long)(from_seconds_ago * m_World->GameWindow()->Timer()->GetFramerate());
    auto after_timestamp = m_World->GetTick() - ticks_ago;
    for (auto effect : m_Effects) {
        if (!effect->IsActive())
            continue;

        if (effect->GetActivatedTimestamp() >= after_timestamp)
            return true;
    }

    return false;
}

void ErrorStatuses::Tick() {
    for (auto effect : m_Effects) {
        if (!effect->IsActive())
            continue;

        if (m_World->GetTick() >= effect->GetEndTimestamp())
            effect->Deactivate();
    }
}

void ErrorStatuses::Draw() {
    if (m_Parent == nullptr)
        return;

    auto current_tick = m_World->GetTick();
    auto directional = (DirectionalEntity*)m_Parent;
    auto parent_offset = directional->GetDirectionalCore().Pos + Vec2d(directional->GetDirectionalCore().sizeRatio, 0.0);

    auto effects_it = m_Effects.begin();
    auto frames_it = m_Frames.Frames.begin();
    while (effects_it != m_Effects.end()) {
        auto effect = *effects_it;

        effects_it++;
        if (!effect->IsActive())
            continue;

        auto& frame = *frames_it;
        auto percentage = (double)(effect->GetEndTimestamp() - current_tick) / (double)effect->GetEffectDuration();
        SDL_Rect source = {
            0,
            0,
            effect->GetTexture()->GetWidth(),
            (int)(effect->GetTexture()->GetHeight() * percentage),
        };
        SDL_FRect frame_rect = {
            (float)(parent_offset.x + frame.Pos.x + 5.0),
            (float)(parent_offset.y + frame.Pos.y),
            (float)(frame.Size.x),
            (float)(frame.Size.y),
        };
        SDL_FRect effect_rect = {
            (float)(parent_offset.x + frame.Pos.x + 5.0),
            (float)(parent_offset.y + frame.Pos.y),
            (float)(frame.Size.x),
            (float)(frame.Size.y * percentage),
        };
        m_Drawing->RenderTextureFCamera(sTextureErrorBackground.GetTexture()->SDLTexture(), nullptr, frame_rect);
        m_Drawing->RenderTextureFCamera(effect->GetTexture()->SDLTexture(), &source, effect_rect);
        m_Drawing->RenderTextureFCamera(sTextureErrorFrame.GetTexture()->SDLTexture(), nullptr, frame_rect);

        frames_it++;
    }
}