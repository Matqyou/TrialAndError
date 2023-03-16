#ifndef TRIALANDERROR_CHARACTER_H
#define TRIALANDERROR_CHARACTER_H
#include <SDL.h>

class Character{
private:
    SDL_Renderer* m_Renderer;
    double m_x, m_y, m_w, m_h;

public:
    Character(SDL_Renderer* Renderer, double starting_x, double starting_y);

    void Draw();
};


#endif //TRIALANDERROR_CHARACTER_H
