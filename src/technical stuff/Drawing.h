//
// Created by Matq on 26/03/2023.
//

#ifndef TRIALANDERROR_SRC_TECHNICAL_STUFF_DRAWING_H_
#define TRIALANDERROR_SRC_TECHNICAL_STUFF_DRAWING_H_

#include "ImageManager.h"
#include <SDL_image.h>

class GameReference;
class GameWorld;
class Drawing {
private:
    GameReference* m_GameWindow;
    GameWorld* m_World;
    SDL_Renderer* m_Renderer;

    double TranslateX(double x);
    double TranslateY(double y);
    SDL_Rect TranslateRect(const SDL_Rect& rect);
    SDL_FRect TranslateFRect(const SDL_FRect& rect);
public:
    explicit Drawing(GameReference* game_window);
    ~Drawing();

    GameWorld* World() const { return m_World; }
    SDL_Renderer* Renderer() const { return m_Renderer; }

    void SetWorld(GameWorld* world);
    void SetRenderTarget(Texture* target);
    void SetBlendingMode(SDL_BlendMode blend_mode);  // Ability to change blending modes
    void LineWorld(double x1, double y1, double x2, double y2);  // Lets you draw a line in the world
    void DrawRectWorld(const SDL_Rect& rect);  // Lets you draw an int rectangle in the world
    void FillRect(const SDL_Rect& rect);  // Lets you fill a rectangle
    void FillRectWorld(const SDL_Rect& rect);  // Lets you fill an int rectangle in the world
    void FillRectFWorld(const SDL_FRect& rect);  // Lets you fill a float rectangle in the world
    void RenderTexture(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_Rect& dstrect);
    void RenderTextureWorld(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_Rect& dstrect);  // Lets you render a basic texture
    void RenderTextureExWorld(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_Rect& dstrect, double angle, SDL_Point* center, SDL_RendererFlip flip);
    void RenderTextureFullscreen(SDL_Texture*, SDL_Rect* srcrect);
    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);  // Sets the color of the renderer
    void Clear();  // Resets all pixels to a color value
    void BlendFullscreen();  // If color blending is enabled, fill the window with a semi-transparent tone
    void UpdateWindow();  // Pushes the latest state of the pixels to window
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_DRAWING_H_
