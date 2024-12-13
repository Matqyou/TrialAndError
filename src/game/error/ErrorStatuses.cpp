//
// Created by Matq on 13/12/2024.
//

#include "ErrorStatuses.h"

LoadedTexture ErrorStatuses::sTextureErrorFrame("interface.ingame.error_frame");
LoadedTexture ErrorStatuses::sTextureErrorBackground("interface.ingame.error_background");

LoadedTexture ErrorStatuses::sTextureFrame("interface.frame");

void ErrorFrames::CreateFrames(std::vector<ErrorStatusEffect*>& group) {
    auto num_elements = (int)group.size();
    const int columns = 2;
    const int rows = (int)(ceil((double)num_elements / columns));
    const int spacing = 1;
    const int frame_size = 20;
    const int iterate_size = frame_size + spacing;

    const int container_width = spacing + iterate_size * columns;
    const int container_height = spacing + iterate_size * rows;
    const int container_height2 = container_height / 2;

    int index = 0;
    for (auto effect : group) {
        int x = index % columns;
        int y = index / columns;

        ErrorFrame new_frame;
        new_frame.Pos = Vec2i(spacing + x * iterate_size, spacing + y * iterate_size - container_height2);
        new_frame.Size = Vec2i(frame_size, frame_size);
        Frames.push_back(new_frame);

        index++;
    }
}

void ErrorFrames::CreateFramesUI(std::vector<ErrorStatusEffect*>& group) {
    auto num_elements = (int)group.size();
    const int spacing = 1;
    const int frame_size = 64;
    const int iterate_size = frame_size + spacing;

    const int container_width = spacing + iterate_size * num_elements;
    const int container_height = spacing + iterate_size;

    int index = -1;
    for (auto effect : group) {
        ErrorFrame new_frame;
        new_frame.Pos = Vec2i(spacing + index * iterate_size, spacing);
        new_frame.Size = Vec2i(frame_size, frame_size);
        Frames.push_back(new_frame);

        index--;
    }
}

ErrorStatuses::ErrorStatuses(Interface* interface, Character* parent, bool gui)
    : InterfaceElement(interface, gui, true),
      BulletFrenzy(this),
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
    m_Parent = parent;
    m_Gui = gui;

    m_Drawing = interface->GameWindow()->Render();

    if (gui) m_Frames.CreateFramesUI(m_Effects);
    else m_Frames.CreateFrames(m_Effects);
}

bool ErrorStatuses::AnyActive(double from_seconds_ago) {
    GameReference* game_window = m_Interface->GameWindow();

    auto ticks_ago = (unsigned long long)(from_seconds_ago * game_window->Timer()->GetFramerate());
    auto after_timestamp = game_window->World()->GetTick() - ticks_ago;
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

        if (m_Interface->GameWindow()->World()->GetTick() >= effect->GetEndTimestamp())
            effect->Deactivate();
    }
}

void ErrorStatuses::Draw() {
    if (!m_Gui) Draw1();
    else Draw2();
}

void ErrorStatuses::Draw1() {
    if (m_Parent == nullptr)
        return;

    auto current_tick = m_Interface->GameWindow()->World()->GetTick();
    auto directional = (DirectionalEntity*)m_Parent;
    auto
        parent_offset = directional->GetDirectionalCore().Pos + Vec2d(directional->GetDirectionalCore().sizeRatio, 0.0);

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

void ErrorStatuses::Draw2() {
    if (m_Parent == nullptr)
        return;

    GameReference* game_window = m_Interface->GameWindow();

    auto current_tick = game_window->World()->GetTick();
    auto gui_offset = Vec2d(game_window->GetWidth(), 0.0);

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
            (float)(gui_offset.x + frame.Pos.x),
            (float)(gui_offset.y + frame.Pos.y),
            (float)(frame.Size.x),
            (float)(frame.Size.y),
        };
        SDL_FRect effect_rect = {
            (float)(gui_offset.x + frame.Pos.x),
            (float)(gui_offset.y + frame.Pos.y),
            (float)(frame.Size.x),
            (float)(frame.Size.y * percentage),
        };
        m_Drawing->RenderTextureF(sTextureErrorBackground.GetTexture()->SDLTexture(), nullptr, frame_rect);
        m_Drawing->RenderTextureF(effect->GetTexture()->SDLTexture(), &source, effect_rect);
        m_Drawing->RenderTextureF(sTextureErrorFrame.GetTexture()->SDLTexture(), nullptr, frame_rect);

        frames_it++;
    }
}