//
// Created by 11dpjgailis on 16.03.2023.
//

#include "Character.h"
#include <cmath>

static double sDiagonalLength = 1.0 / std::sqrt(2.0);
static int sNumCharacters = 0;
const int Character::saControlsPlayer1[NUM_CONTROLS] = {SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_A };
const int Character::saControlsPlayer2[NUM_CONTROLS] = {SDL_SCANCODE_UP, SDL_SCANCODE_RIGHT, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT };
const int Character::saControlsPlayer3[NUM_CONTROLS] = {SDL_SCANCODE_I, SDL_SCANCODE_L, SDL_SCANCODE_K, SDL_SCANCODE_J };

Character::Character(SDL_Renderer* Renderer, double start_x, double start_y)
 : Entity(Renderer, start_x, start_y, 50, 50) {
    for (bool& State : m_Movement)
        State = false;

    sNumCharacters += 1;
    const int* paControls;
    switch (sNumCharacters) {
        default: {
            paControls = nullptr;
        } break;
        case 1: {
            paControls = saControlsPlayer1;
        } break;
        case 2: {
            paControls = saControlsPlayer2;
        } break;
        case 3: {
            paControls = saControlsPlayer3;
        } break;
    }
    if (paControls) { memcpy(m_aControls, paControls, sizeof(m_aControls)); }
    else { memset(m_aControls, 0, sizeof(m_aControls)); }
    m_Controllable = bool(paControls);

    m_xvel = 0.0;
    m_yvel = 0.0;
}

void Character::TickControls() {
    // Check if buttons are held
    const double Speed = 0.5;
    bool MoveUp = m_Movement[CONTROL_UP];
    bool MoveRight = m_Movement[CONTROL_RIGHT];
    bool MoveDown = m_Movement[CONTROL_DOWN];
    bool MoveLeft = m_Movement[CONTROL_LEFT];

    bool Horizontally = MoveLeft != MoveRight;
    bool Vertically = MoveUp != MoveDown;

    // Accelerate when buttons are held
    double LengthPerAxis = (Horizontally && Vertically) ? sDiagonalLength : 1.0;
    double SpeedPerAxis = Speed * LengthPerAxis;

    if (MoveDown != MoveUp) m_yvel += SpeedPerAxis * double(MoveDown ? 1 : -1);
    if (MoveRight != MoveLeft) m_xvel += SpeedPerAxis * double(MoveRight ? 1 : -1);
}

void Character::TickVelocity() {
    m_x += m_xvel;
    m_y += m_yvel;
}

void Character::Tick() {
    TickControls();  // Do stuff depending on the current held buttons
    TickVelocity();  // Move the chracter entity
}

void Character::Event(const SDL_Event& CurrentEvent) {
    if (!m_Controllable)
        return;

    if (CurrentEvent.type == SDL_KEYDOWN ||
        CurrentEvent.type == SDL_KEYUP) {
        bool State = CurrentEvent.type == SDL_KEYDOWN;
        for (int i = 0; i < NUM_CONTROLS; i++) {
            if (CurrentEvent.key.keysym.scancode == m_aControls[i])
                m_Movement[i] = State;
        }
    }
}