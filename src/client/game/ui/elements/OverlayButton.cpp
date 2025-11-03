//
// Created by Matq on 13/01/2025.
//

#include "OverlayButton.h"
#include <client/core/Application.h>

OverlayButton::OverlayButton(Texture* texture, Texture* pressed_texture, Texture* overlay_texture)
    : Button(texture, pressed_texture),
      overlay_texture_instance(overlay_texture) {
    this->name = "OverlayButton";
}

void OverlayButton::PostRefresh() {
    if (draw == DRAW_TEXTURE) {
        UpdateTexturePlacement();
        UpdatePressedVisualTexture();
        UpdateOverlayTexture();
    }
}

void OverlayButton::Render() {
    if (draw != ElementDraw::DONT_DRAW) {
        auto drawing = Application.GetDrawing();
        auto& fill_color = has_focus ? focus_color : color;

        if (draw == ElementDraw::DRAW_RECT) {
            drawing->SetColor(fill_color);
            drawing->DrawRect(GetRect(), true);
        } else if (draw == ElementDraw::DRAW_TEXTURE) {
            SDL_Texture* pressed_sdl_texture = nullptr;
            if (pressed_texture_instance.GetTexture() != nullptr)
                pressed_sdl_texture = pressed_texture_instance.GetTexture()->SDLTexture();

            if (pressed_down && pressed_sdl_texture != nullptr) {
                drawing->RenderTexture(pressed_sdl_texture,
                                       nullptr, pressed_texture_instance.GetResultingFRect());
            } else {
                drawing->RenderTexture(texture_instance.GetTexture()->SDLTexture(),
                                       nullptr, texture_instance.GetResultingFRect());
            }
            drawing->RenderTexture(overlay_texture_instance.GetTexture()->SDLTexture(),
                                   nullptr, overlay_texture_instance.GetResultingFRect());
        }
    }

    RenderChildren();
}

void OverlayButton::UpdateOverlayTexture() {
    overlay_texture_instance.UpdateWithNewPlacement(GetRect());
}