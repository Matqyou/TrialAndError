#ifndef TRIALANDERROR_CHARACTER_H
#define TRIALANDERROR_CHARACTER_H

#include <SDL.h>
#include "Entity.h"

class Character : public Entity {
private:
    enum {
        CONTROL_UP,
        CONTROL_RIGHT,
        CONTROL_DOWN,
        CONTROL_LEFT,
        NUM_CONTROLS
    };
    bool m_Movement[NUM_CONTROLS];
    int m_Controls[NUM_CONTROLS];
    bool m_Controllable;

    static const int sControlsPlayer1[NUM_CONTROLS];
    static const int sControlsPlayer2[NUM_CONTROLS];
    static const int sControlsPlayer3[NUM_CONTROLS];

    double m_xvel, m_yvel;  // velocity

    void TickControls();
    void TickVelocity();

public:
    Character(SDL_Renderer* Renderer, double start_x, double start_y);

    // void Draw();
    void Tick();
    void Event(const SDL_Event& CurrentEvent);
};


#endif //TRIALANDERROR_CHARACTER_H
