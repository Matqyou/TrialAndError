//
// Created by Matq on 20/03/2023.
//

#pragma once

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
    explicit GameController(int device_id);
    ~GameController();

    // Getting
    [[nodiscard]] int InstanceID() const { return m_InstanceID; }
    [[nodiscard]] double GetLeftTrigger() const { return m_Axis[SDL_CONTROLLER_AXIS_TRIGGERLEFT]; }
    [[nodiscard]] double GetRightTrigger() const { return m_Axis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT]; }
    [[nodiscard]] bool GetButton(int button) const { return m_Buttons[button]; }
    [[nodiscard]] bool GetLastButton(int button) const { return m_LastButtons[button]; }

    // Generating
    void GetJoystick1(double& get_x, double& get_y);
    void GetJoystick2(double& get_x, double& get_y);

    // Manipulate
    bool Vibrate(int low_frequency_rumble, int high_frequency_rumble, int duration_ms);

    // Listening & Ticking
    void Event(const SDL_Event& event);
    void TickReset();
};

class GameControllers {
private:
    std::vector<GameController*> m_Controllers;

public:
    GameControllers();
    ~GameControllers();

    // Manipulating & Generating
    [[nodiscard]] GameController* OpenController(int device_id);
    GameController* CloseController(int instance_id);

    // Listening & Ticking
    void Event(const SDL_Event& event);
    void TickReset();
};
