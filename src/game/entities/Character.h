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
#include "../indicators/HealthBar.h"

class Character;
struct Hook {
    enum {
        GRABBED_NONE,
        GRABBED_ENTITY,
        GRABBED_WALL
    };

    Character* m_Parent;
    double m_x, m_y;
    double m_xvel, m_yvel;
    double m_MaxLength;
    double m_HookTravelSpeed;
    double m_WallDragForce;
    double m_HookerInfluenceRatio;
    double m_HookistInfluenceRatio;
    bool m_Deployed;
    int m_Grabbed;
    Entity* m_GrabbedEntity;

    Hook(Character* parent);

    void SetInfluenceRatio(double ratio);
    void Unhook();
    void HookWall();

    void Tick(bool hooking, bool last_hooking);
};

class Character : public Entity {
public:
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
    friend class Hook;
    int m_PlayerIndex;
    std::string m_Name;
    Texture* m_Nameplate;
    double m_ColorHue;
    GameController* m_GameController;
    bool m_Movement[NUM_CONTROLS] {};
    int m_Controls[NUM_CONTROLS] {};
    bool m_Controllable;
    bool m_Shooting, m_LastShooting;
    // std::vector<ProjectileWeapon*> m_Weapons; Option to have multiple weapons of same type, dont think we need it yet
    ProjectileWeapon* m_Weapons[NUM_WEAPONS] {};
    ProjectileWeapon* m_CurrentWeapon;
    double m_MaxHealth, m_Health;
    static const int sDefaultControls[NUM_CONTROLS];
    const double m_BaseAcceleration;
    double m_xLook, m_yLook;  // direction
    Hook m_Hook;
    bool m_Hooking, m_LastHooking;
    bool m_Reloading, m_LastReloading;
    int m_HitTicks;
    HealthBar m_HealthBar;

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

    Hook* GetHook() { return &m_Hook; }
    GameController* GetGameController() const { return m_GameController; }
    int PlayerIndex() const { return m_PlayerIndex; }

    void SetGameController(GameController* gameController);
    void Accelerate(double accelerate_x, double accelerate_y);

    void Event(const SDL_Event& currentEvent);
    void Tick() override;
    void Draw() override;


};


#endif //TRIALANDERROR_CHARACTER_H
