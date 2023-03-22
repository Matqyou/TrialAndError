//
// Created by 11dpjgailis on 16.03.2023.
//
#include "Character.h"
#include <cmath>
#include <iostream>
static double sDiagonalLength = 1.0 / std::sqrt(2.0);
static int sNumCharacters = 0;
const int Character::sDefaultControls[NUM_CONTROLS] = {SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_A };

Character::Character(GameReference* gameWindow, double start_x, double start_y)
 : Entity(gameWindow, start_x, start_y, 50, 50) {
    sNumCharacters += 1;
    char Name[CHARACTER_MAX_NAME_LENGTH];
    std::snprintf(Name, CHARACTER_MAX_NAME_LENGTH, "Player%i", sNumCharacters);
    m_Name = Name;

    m_GameController = nullptr;
    for (bool& State : m_Movement)
        State = false;

    if (sNumCharacters == 1) { memcpy(m_Controls, sDefaultControls, sizeof(m_Controls)); }  // Controls are copied
    else { memset(m_Controls, 0, sizeof(m_Controls)); }  // All controls are set to 0
    m_Controllable = true;

    m_xvel = 0.0;
    m_yvel = 0.0;
    m_xlook = 1.0;
    m_ylook = 0.0;
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

    // Update look direction
    int XMouse, YMouse;
    SDL_GetMouseState(&XMouse, &YMouse);

    m_xlook = XMouse - m_x;
    m_ylook = YMouse - m_y;
    double Distance = std::sqrt(std::pow(m_xlook, 2) + std::pow(m_ylook, 2));

    if (Distance != 0.0) {
        m_xlook /= Distance;
        m_ylook /= Distance;
    } else {
        m_xlook = 1.0;
        m_ylook = 0.0;
    }
}

void Character::TickGameControllerControls() {
    // Check for current joystick values
    double AxisX, AxisY;
    m_GameController->GetJoystick1(AxisX, AxisY);

    // AxisX**2 + AxisY**2 <= 1 (keep direction length of 1)
    double Length = std::sqrt(std::pow(AxisX, 2) + std::pow(AxisY, 2));
    if (Length > 0.1) {  // Fix controller drifting
        if (Length > 0.0) {
            AxisX /= Length;
            AxisY /= Length;
        }

        // Accelerate in that direction
        m_xvel += m_BaseAcceleration * AxisX;
        m_yvel += m_BaseAcceleration * AxisY;
    }

    // Update look direction
    double AxisX2, AxisY2;
    m_GameController->GetJoystick2(AxisX2, AxisY2);

    Length = std::sqrt(std::pow(AxisX2, 2) + std::pow(AxisY2, 2));
    if (Length > 0.6) {  // Fix controller drifting
        if (Length != 0.0) {
            m_xlook = AxisX2 / Length;
            m_ylook = AxisY2 / Length;
        } else {
            m_xlook = 1.0;
            m_ylook = 0.0;
        }
    }
}

void Character::TickControls() {
    if (!m_Controllable)
        return;

    if (m_GameController)
        TickGameControllerControls();
    else
        TickKeyboardControls();
}

void Character::TickVelocity() {
    m_xvel *= m_BaseDamping;
    m_yvel *= m_BaseDamping;

    if((m_x < 900-25) and (m_x > 25)) m_x += m_xvel; // if on screen
    else if (m_x >= 900-25) m_x -= 5; // if going to the right
    else if (m_x <= 25)m_x += 5; // if going to the left
    if((m_y < 700-25) and (m_y > 25)) m_y += m_yvel; // if on screen
    else if(m_y >= 700-25) m_y -= 5; // if going below screen
    else if (m_y <= 25)m_y += 5; // if going above screen
}

void Character::Draw() {
    SDL_Renderer* Renderer = m_GameWindow->Renderer();

    SDL_FRect DrawRect = {float(m_x) - float(m_w/2),
                          float(m_y) - float(m_h/2),
                          float(m_w),
                          float(m_h)};
    SDL_SetRenderDrawColor(Renderer, rand()%255, rand()%255, rand()%255, 255);
    SDL_RenderFillRectF(Renderer, &DrawRect);

    double XLook = m_x + m_xlook * 100.0;
    double YLook = m_y + m_ylook * 100.0;
    SDL_SetRenderDrawColor(Renderer, rand()%255, rand()%255, rand()%255, 255);
    SDL_RenderDrawLine(Renderer, int(m_x), int(m_y), int(XLook), int(YLook));
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