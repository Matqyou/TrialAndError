//
// Created by Matq on 26/03/2023.
//

#ifndef TRIALANDERROR_SRC_TECHNICAL_STUFF_DRAWING_H_
#define TRIALANDERROR_SRC_TECHNICAL_STUFF_DRAWING_H_

#include <SDL_image.h>

class Drawing {
private:
    SDL_Renderer* m_Renderer;
    double m_x, m_y;

public:
    explicit Drawing(SDL_Renderer* renderer);
    ~Drawing();

    SDL_Renderer* Renderer() const { return m_Renderer; }

    void SetCameraPos(double x, double y);
    void SetBlendingMode(SDL_BlendMode blend_mode);
    void FillRect(SDL_Rect* rect);
    void RenderTexture(SDL_Texture* texture, SDL_Rect* srcrect, SDL_Rect* dstrect);
    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void Clear();
    void UpdateWindow();
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_DRAWING_H_
