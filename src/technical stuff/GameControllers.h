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

    double m_Axis[4];

public:
    GameController(int device_id);
    ~GameController();

    int InstanceID() const { return m_InstanceID; }
    void GetJoystick1(double& get_x, double& get_y);
    void GetJoystick2(double& get_x, double& get_y);

    bool Vibrate(int low_frequency_rumble, int high_frequency_rumble, int duration_ms);
    bool VibrateTriggers(int left_rumble, int right_rumble, int duration_ms);

    void Event(const SDL_Event& event);
};

class GameControllers {
private:
    std::vector<GameController*> m_Controllers;

public:
    GameControllers();
    ~GameControllers();

    GameController* OpenController(int device_id);
    GameController* CloseController(int instance_id);

    void Event(const SDL_Event& event);
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMECONTROLLERS_H_
