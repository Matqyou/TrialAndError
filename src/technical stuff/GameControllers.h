//
// Created by Matq on 20/03/2023.
//

#ifndef TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMECONTROLLERS_H_
#define TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMECONTROLLERS_H_

#include <SDL.h>
#include <vector>

class GameController {
private:
    SDL_GameController* m_Device;
    int m_InstanceID;

public:
    GameController(int device_id);
    ~GameController();

    int InstanceID() const { return m_InstanceID; }
};

class GameControllers {
private:
    std::vector<GameController*> m_Controllers;

public:
    GameControllers();
    ~GameControllers();

    GameController* OpenController(int device_id);
    void CloseController(int instance_id);
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMECONTROLLERS_H_
