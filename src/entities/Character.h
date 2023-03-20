#ifndef TRIALANDERROR_CHARACTER_H
#define TRIALANDERROR_CHARACTER_H

#include <SDL.h>
#include "Entity.h"
#include "../technical stuff/GameControllers.h"

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
    GameController* m_GameController;

    static const int sControlsPlayer1[NUM_CONTROLS];
    static const int sControlsPlayer2[NUM_CONTROLS];
    static const int sControlsPlayer3[NUM_CONTROLS];

    const double m_BaseAcceleration = 0.5;
    double m_xvel, m_yvel;  // velocity

    void TickKeyboardControls();
    void TickGameControllerControls();
    void TickControls();
    void TickVelocity();

public:
    Character(SDL_Renderer* Renderer, double start_x, double start_y);

    void SetGameController(GameController* gameController);

    // void Draw(); // remove comment if want to draw character differently
    void Tick();
    void Event(const SDL_Event& CurrentEvent);
};


#endif //TRIALANDERROR_CHARACTER_H
