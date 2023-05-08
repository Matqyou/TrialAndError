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

    double m_Axis[SDL_CONTROLLER_AXIS_MAX];
    bool m_Buttons[SDL_CONTROLLER_BUTTON_MAX];
    bool m_LastButtons[SDL_CONTROLLER_BUTTON_MAX];

public:
    GameController(int device_id);
    ~GameController();

    int InstanceID() const { return m_InstanceID; }
    double GetLeftTrigger() const { return m_Axis[SDL_CONTROLLER_AXIS_TRIGGERLEFT]; }
    double GetRightTrigger() const { return m_Axis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT]; }
    void GetJoystick1(double& get_x, double& get_y);
    void GetJoystick2(double& get_x, double& get_y);
    bool GetButton(int button);
    bool GetLastButton(int button);

    bool Vibrate(int low_frequency_rumble, int high_frequency_rumble, int duration_ms);
    bool VibrateTriggers(int left_rumble, int right_rumble, int duration_ms); // TODO not work

    void Event(const SDL_Event& event);
    void TickLast();
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
    void TickLast();
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMECONTROLLERS_H_
