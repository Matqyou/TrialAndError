//
// Created by 11dpjgailis on 16.03.2023.
//

#include "Character.h"
Character::Character(SDL_Renderer* Renderer, double start_x, double start_y)
 : Entity(Renderer, start_x, start_y, 50, 50) {

    for (bool& State : m_Movement)
        State = false;
}

void Character::Tick() {
    double Speed = 0.05;
    if (m_Movement[MOVEMENT_UP])
        m_y -= Speed;
}

void Character::Event(const SDL_Event& CurrentEvent) {
    if (CurrentEvent.type == SDL_KEYDOWN ||
        CurrentEvent.type == SDL_KEYUP) {
        bool State = CurrentEvent.type == SDL_KEYDOWN;
        switch (CurrentEvent.key.keysym.scancode) {
            case SDL_SCANCODE_W: {
                m_Movement[MOVEMENT_UP] = State;
            } break;
            case SDL_SCANCODE_D: {
                m_Movement[MOVEMENT_RIGHT] = State;
            } break;
            case SDL_SCANCODE_S: {
                m_Movement[MOVEMENT_DOWN] = State;
            } break;
            case SDL_SCANCODE_A: {
                m_Movement[MOVEMENT_LEFT] = State;
            } break;
            default: break;
        }
    }
}