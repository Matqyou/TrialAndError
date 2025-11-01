//
// Created by Matq on 16/12/2024.
//

#pragma once

#include "client/game/ui/elements/Element.h"
#include "shared/utility/Functions.h"
#include <utility>

class Button : public Element {
protected:
    Callback callback;
    bool clickable;

    VisualTextureInstance pressed_texture_instance;
    bool pressed_down;

public:
    Button();
    Button(Texture* texture, Texture* pressed_texture);

    // Options
    Button* SetCallback(Callback new_callback) {
        this->callback = std::move(new_callback);
        return this;
    }
    Button* SetClickable(bool new_clickable) {
        this->clickable = new_clickable;
        return this;
    }
    Button* SetPressedTexture(Texture* texture) {
        this->pressed_texture_instance.ChangeTexture(texture);
        return this;
    }

    // Manipulating
    void RunCallback() const;

    // Ticking
    void PostRefresh() override;
    void HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary) override;
    void Render() override;
    void UpdatePressedVisualTexture();

};