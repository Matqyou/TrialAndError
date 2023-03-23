//
// Created by Matq on 20/03/2023.
//

#include "GameControllers.h"
#include <iostream>

GameController::GameController(int device_id) {
    m_Device = SDL_GameControllerOpen(device_id);
    SDL_Joystick* Joystick = SDL_GameControllerGetJoystick(m_Device);
    m_InstanceID = SDL_JoystickInstanceID(Joystick);

    for (auto& Axis : m_Axis)
        Axis = 0;

    for (bool& Button : m_Buttons)
        Button = false;
}

GameController::~GameController() {
    SDL_GameControllerClose(m_Device);
}

void GameController::GetJoystick1(double& get_x, double& get_y) {
    get_x = m_Axis[0];
    get_y = m_Axis[1];
}

void GameController::GetJoystick2(double& get_x, double& get_y) {
    get_x = m_Axis[2];
    get_y = m_Axis[3];
}

bool GameController::GetButton(int button) {
    return m_Buttons[button];
}

bool GameController::Vibrate(int low_frequency_rumble, int high_frequency_rumble, int duration_ms) {
    return SDL_GameControllerRumble(m_Device, low_frequency_rumble, high_frequency_rumble, duration_ms);
}

bool GameController::VibrateTriggers(int left_rumble, int right_rumble, int duration_ms) {
    return SDL_GameControllerRumbleTriggers(m_Device, left_rumble, right_rumble, duration_ms);
}

void GameController::Event(const SDL_Event& event) {
    switch (event.type) {
        case SDL_CONTROLLERBUTTONUP:
        case SDL_CONTROLLERBUTTONDOWN: {
            bool Pressed = event.type == SDL_CONTROLLERBUTTONDOWN;
            m_Buttons[event.cbutton.button] = Pressed;
        } break;
        case SDL_CONTROLLERAXISMOTION: {
            int AxisID = event.caxis.axis;
            double& CurrentAxis = m_Axis[AxisID];
            CurrentAxis = double(event.caxis.value) / SDL_MAX_SINT16;
        } break;
    }
}

GameControllers::GameControllers() {

}

GameControllers::~GameControllers() {
    for (GameController* Controller : m_Controllers)
        delete Controller;
}

GameController *GameControllers::OpenController(int device_id) {
    auto* NewController = new GameController(device_id);
    m_Controllers.push_back(NewController);
    return NewController;
}

GameController* GameControllers::CloseController(int instance_id) {
    for (auto Iterator = m_Controllers.begin(); Iterator != m_Controllers.end(); Iterator++) {
        GameController* Controller = *Iterator;
        if (Controller->InstanceID() != instance_id)
            continue;

        m_Controllers.erase(Iterator);
        delete Controller;
        return Controller;
    }

    return nullptr;
}

void GameControllers::Event(const SDL_Event& event) {
    int InstanceID = event.cdevice.which;
    for (GameController* Controller : m_Controllers) {
        if (Controller->InstanceID() == InstanceID) {
            Controller->Event(event);
            break;
        }
    }
}
