//
// Created by Matq on 20/03/2023.
//

#include "GameControllers.h"
#include <iostream>

GameController::GameController(int device_id) {
    m_Device = SDL_GameControllerOpen(device_id);
    SDL_Joystick* Joystick = SDL_GameControllerGetJoystick(m_Device);
    m_InstanceID = SDL_JoystickInstanceID(Joystick);
}

GameController::~GameController() {
    SDL_GameControllerClose(m_Device);
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

void GameControllers::CloseController(int instance_id) {
    for (auto Iterator = m_Controllers.begin(); Iterator != m_Controllers.end(); Iterator++) {
        GameController* Controller = *Iterator;
        if (Controller->InstanceID() != instance_id)
            continue;

        m_Controllers.erase(Iterator);
        delete Controller;
        break;
    }
}
