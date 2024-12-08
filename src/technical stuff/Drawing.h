//
// Created by Matq on 26/03/2023.
//

#pragma once

#include <SDL_image.h>
#include "../client/Assets.h"

class GameReference;
class GameWorld;
class Drawing {
private:
    GameReference* m_GameWindow;
    GameWorld* m_World;
    SDL_Renderer* m_Renderer;
    double m_CameraX, m_CameraY;
    double m_Zoom;

public:
    explicit Drawing(GameReference* game_window);
    ~Drawing();

    // Getting
    [[nodiscard]] GameWorld* World() const { return m_World; }
    [[nodiscard]] SDL_Renderer* Renderer() const { return m_Renderer; }
    [[nodiscard]] double GetCameraX() const { return m_CameraX; }
    [[nodiscard]] double GetCameraY() const { return m_CameraY; }
    [[nodiscard]] double GetZoom() const { return m_Zoom; }

    // Generating
    double DetranslateX(double x);
    double DetranslateY(double y);
    double TranslateX(double x);
    double TranslateY(double y);
    SDL_Rect TranslateRect(const SDL_Rect& rect);
    SDL_FRect TranslateFRect(const SDL_FRect& rect);

    // Setting
    void SetCameraPos(double camera_x, double camera_y);
    void SetZoom(double zoom) { m_Zoom = zoom; }
    void SetWorld(GameWorld* world) { m_World = world; }
    void SetDrawBlendMode(SDL_BlendMode blend_mode) { SDL_SetRenderDrawBlendMode(m_Renderer, blend_mode); }
    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { SDL_SetRenderDrawColor(m_Renderer, r, g, b, a); }
    void SetColor(SDL_Color color) { SetColor(color.r, color.g, color.b, color.a); }
    void SetRenderTarget(Texture* target);

    // Drawing
    void FillRect(const SDL_Rect& rect);
    void RenderTexture(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_Rect& dstrect);
    void RenderTextureF(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_FRect& dstrect);
    void RenderTextureFullscreen(SDL_Texture*, SDL_Rect* srcrect);
    void Clear();  // Resets all pixels to a color value
    void FillAll();  // If color blending is enabled, fill the window with a semi-transparent tone
    void UpdateWindow();  // Pushes the latest state of the pixels to window

    // Drawing in camera coordinates
    void LineCamera(double x1, double y1, double x2, double y2);
    void DrawRectCamera(const SDL_Rect& rect);
    void FillRectCamera(const SDL_Rect& rect);
    void FillRectFCamera(const SDL_FRect& rect);
    void RenderTextureCamera(SDL_Texture* texture,
                             SDL_Rect* srcrect,
                             const SDL_Rect& dstrect);
    void RenderTextureExCamera(SDL_Texture* texture,
                               SDL_Rect* srcrect,
                               const SDL_Rect& dstrect,
                               double angle,
                               SDL_Point* center,
                               SDL_RendererFlip flip);
    void RenderTextureFCamera(SDL_Texture* texture,
                              SDL_Rect* srcrect,
                              const SDL_FRect& dstrect);
    void RenderTextureExFCamera(SDL_Texture* texture,
                                SDL_Rect* srcrect,
                                const SDL_FRect& dstrect,
                                double angle,
                                SDL_FPoint* center,
                                SDL_RendererFlip flip);
    void RenderDebugPointCamera(double x, double y);
};
