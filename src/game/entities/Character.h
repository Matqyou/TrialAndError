#ifndef TRIALANDERROR_CHARACTER_H
#define TRIALANDERROR_CHARACTER_H

#include "SDL.h"
#include <cstring>
#include "Entity.h"
#include "../weapons/WeaponGlock.h"
#include "../weapons/WeaponShotgun.h"
#include "../weapons/WeaponBurst.h"
#include "../weapons/WeaponMinigun.h"
#include "../../technical stuff/GameControllers.h"
#include "../../technical stuff/Colors.h"

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
    friend class ProjectileWeapon;
    friend class Bullets;
    int m_PlayerIndex;
    std::string m_Name;
    Texture* m_Nameplate;
    double m_ColorHue;
    GameController* m_GameController;
    bool m_Movement[NUM_CONTROLS];
    int m_Controls[NUM_CONTROLS];
    bool m_Controllable;
    WeaponType m_Weapon; // TODO: Make this a pointer and store available guns in a list/vector
    bool m_Shoot, m_LastShoot;
    WeaponGlock m_Glock;
    WeaponShotgun m_Shotgun;
    WeaponBurst m_Burst;
    WeaponMinigun m_Minigun;
    static const int sDefaultControls[NUM_CONTROLS];
    const double m_BaseAcceleration = 0.75;
    double m_xLook, m_yLook;  // direction
    int hp;
    double m_xHook, m_yHook;
    double m_xvelHook, m_yvelHook;
    bool m_HookDeployed;
    bool m_Hook, m_LastHook;
    bool m_HookGrabbedWall;
    int is_hit;
    void TickKeyboardControls();
    void TickGameControllerControls();
    void TickControls();
    void TickHook();
    void TickWeapon();

public:
    static Sound* ch_HitSound;
    static Sound* ch_DeathSound;
    Character(GameWorld* world, double start_x, double start_y, double start_xvel, double start_yvel);
    ~Character();

    GameController* GetGameController() const { return m_GameController; }
    int PlayerIndex() const { return m_PlayerIndex; }

    void SetGameController(GameController* gameController);
    void Accelerate(double accelerate_x, double accelerate_y);

    void Event(const SDL_Event& currentEvent);
    void Tick() override;
    void Draw() override;


};


#endif //TRIALANDERROR_CHARACTER_H
