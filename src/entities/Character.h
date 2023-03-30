#ifndef TRIALANDERROR_CHARACTER_H
#define TRIALANDERROR_CHARACTER_H

#include <SDL.h>
#include <cstring>
#include "Entity.h"
#include "../technical stuff/GameControllers.h"
#include "../technical stuff/Colors.h"

class Character : public Entity {
public:
    enum WeaponType{
        WEAPON_NONE,
        WEAPON_FIST,
        WEAPON_GLOCK,
        WEAPON_BURST,
        WEAPON_SHOTGUN,
        WEAPON_MACHINEGUN,
        NUM_WEAPONS
    };
    enum {
        CONTROL_UP,
        CONTROL_RIGHT,
        CONTROL_DOWN,
        CONTROL_LEFT,
        NUM_CONTROLS,
        CHARACTER_MAX_NAME_LENGTH = 32
    };

private:
    friend class GameWorld;
    int m_PlayerIndex;
    std::string m_Name;
    Texture* m_Nameplate;
    double m_ColorHue;
    GameController* m_GameController;
    bool m_Movement[NUM_CONTROLS];
    int m_Controls[NUM_CONTROLS];
    bool m_Controllable;
    WeaponType m_Weapon;
    Sound* m_ShootSound;
    bool m_Shoot;
    double m_MachinegunTick;
    unsigned long long burst_ticks;
    int m_BurstShots;
    int m_StartBurstShots;
    static const int sDefaultControls[NUM_CONTROLS];

    const double m_BaseAcceleration = 0.75;
    double m_xLook, m_yLook;  // direction
    unsigned long long m_LastShot;

    double m_xHook, m_yHook;
    double m_xvelHook, m_yvelHook;
    bool m_HookDeployed;
    bool m_Hook, m_LastHook;
    bool m_HookGrabbedWall;

    void TickKeyboardControls();
    void TickGameControllerControls();
    void TickControls();
    void TickHook();
    void TickWeapon();

public:
    Character(GameWorld* world, double start_x, double start_y, double start_xvel, double start_yvel);
    ~Character();

    void SetGameController(GameController* gameController);
    GameController* GetGameController() const { return m_GameController; }
    int PlayerIndex() const { return m_PlayerIndex; }

    void Event(const SDL_Event& currentEvent);
    void Tick() override;
    void Draw() override;
};


#endif //TRIALANDERROR_CHARACTER_H
