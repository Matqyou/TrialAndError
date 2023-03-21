//
// Created by 11dpjgailis on 16.03.2023.
//
#include "Character.h"
#include <cmath>
#include <iostream>
static double sDiagonalLength = 1.0 / std::sqrt(2.0);
static int sNumCharacters = 0;
const int Character::sControlsPlayer1[NUM_CONTROLS] = {SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_A };
const int Character::sControlsPlayer2[NUM_CONTROLS] = {SDL_SCANCODE_UP, SDL_SCANCODE_RIGHT, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT };
const int Character::sControlsPlayer3[NUM_CONTROLS] = {SDL_SCANCODE_I, SDL_SCANCODE_L, SDL_SCANCODE_K, SDL_SCANCODE_J };

Character::Character(SDL_Renderer* Renderer, double start_x, double start_y)
 : Entity(Renderer, start_x, start_y, 50, 50) {
    sNumCharacters += 1;
    char Name[CHARACTER_MAX_NAME_LENGTH];
    std::snprintf(Name, CHARACTER_MAX_NAME_LENGTH, "Player%i", sNumCharacters);
    m_Name = Name;

    m_GameController = nullptr;
    for (bool& State : m_Movement)
        State = false;

    const int* paControls;
    switch (sNumCharacters) {
        default: {
            paControls = nullptr;
        } break;
        case 1: {
            paControls = sControlsPlayer1;
        } break;
        case 2: {
            paControls = sControlsPlayer2;
        } break;
        case 3: {
            paControls = sControlsPlayer3;
        } break;
    }
    if (paControls) { memcpy(m_Controls, paControls, sizeof(m_Controls)); }
    else { memset(m_Controls, 0, sizeof(m_Controls)); }
    m_Controllable = bool(paControls);

    m_xvel = 0.0;
    m_yvel = 0.0;
}

void Character::SetGameController(GameController* gameController) {
    m_GameController = gameController;
}

void Character::TickKeyboardControls() {
    // Check if buttons are held
    bool MoveUp = m_Movement[CONTROL_UP];
    bool MoveRight = m_Movement[CONTROL_RIGHT];
    bool MoveDown = m_Movement[CONTROL_DOWN];
    bool MoveLeft = m_Movement[CONTROL_LEFT];

    bool Horizontally = MoveLeft != MoveRight;
    bool Vertically = MoveUp != MoveDown;

    // Accelerate when buttons are held
    double LengthPerAxis = (Horizontally && Vertically) ? sDiagonalLength : 1.0;
    double SpeedPerAxis = m_BaseAcceleration * LengthPerAxis;

    if((m_y < 700-25) and (m_y > 0+25)){
        if (MoveDown != MoveUp) m_yvel += SpeedPerAxis * double(MoveDown ? 1 : -1);
    }
    else m_yvel = 0;
    if((m_x < 900-25) and (m_x > 0+25)){
        if (MoveRight != MoveLeft) m_xvel += SpeedPerAxis * double(MoveRight ? 1 : -1);
    }
    else m_xvel = 0;
}

void Character::TickGameControllerControls() {
    // Check for current joystick values
    double AxisX, AxisY;
    m_GameController->GetJoystick1(AxisX, AxisY);

    // AxisX**2 + AxisY**2 <= 1 (keep direction length of 1)
    double Length = std::sqrt(std::pow(AxisX, 2) + std::pow(AxisY, 2));
    if (Length <= 0.1) // Fix controller drifting
        return;

    if (Length > 0.0) {
        AxisX /= Length;
        AxisY /= Length;
    }

    // Accelerate in that direction
    m_xvel += m_BaseAcceleration * AxisX;
    m_yvel += m_BaseAcceleration * AxisY;
}

void Character::TickControls() {
    if (m_GameController)
        TickGameControllerControls();
    else
        TickKeyboardControls();
}

void Character::TickVelocity() {
    if((m_x < 900-25) and (m_x > 25)) m_x += m_xvel; // if on screen
    else if (m_x >= 900-25) m_x -= 5; // if going to the right
    else if (m_x <= 25)m_x += 5; // if going to the left
    if((m_y < 700-25) and (m_y > 25)) m_y += m_yvel; // if on screen
    else if(m_y >= 700-25) m_y -= 5; // if going below screen
    else if (m_y <= 25)m_y += 5; // if going above screen

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
            if (CurrentEvent.key.keysym.scancode == m_Controls[i])
                m_Movement[i] = State;
        }
    }
}