#ifndef TRIALANDERROR_CHARACTER_H
#define TRIALANDERROR_CHARACTER_H

#include "SDL.h"
#include <cstring>
#include "../Entity.h"
#include "../../weapons/WeaponGlock.h"
#include "../../weapons/WeaponShotgun.h"
#include "../../weapons/WeaponBurst.h"
#include "../../weapons/WeaponMinigun.h"
#include "../../../technical stuff/GameControllers.h"
#include "../../../technical stuff/Colors.h"
#include "../../indicators/HealthBar.h"
#include "../../indicators/TextSurface.h"
#include "Hook.h"

class Character : public Entity {
public:
    enum {
        CONTROL_UP,
        CONTROL_RIGHT,
        CONTROL_DOWN,
        CONTROL_LEFT,
        CONTROL_SHIFT,
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
    TextSurface* m_Nameplate;
    TextSurface* m_CoordinatePlate;
    TextSurface* m_AmmoCount;
    TextSurface* m_HealthInt;
    double m_ColorHue;
    GameController* m_GameController;
    bool m_Movement[NUM_CONTROLS] {};
    int m_Controls[NUM_CONTROLS] {};
    bool m_Controllable;
    bool m_Using, m_LastUsing;
    unsigned long long m_LastFisted;
    unsigned long long m_LastFistedL, m_LastFistedR;
    // std::vector<ProjectileWeapon*> m_Weapons; Option to have multiple weapons of same type, dont think we need it yet
    ProjectileWeapon* m_Weapons[NUM_WEAPONS] {};
    ProjectileWeapon* m_CurrentWeapon;
    double m_MaxHealth, m_Health;
    static const int sDefaultControls[NUM_CONTROLS];
    const double m_BaseAcceleration;
    double m_Acceleration;
    double m_xLook, m_yLook;  // direction
    Hook m_Hook;
    bool m_Hooking, m_LastHooking;
    bool m_Reloading, m_LastReloading;
    int m_HitTicks;
    HealthBar m_HealthBar;

    SDL_Color m_CharacterColor;
    SDL_Color m_HookColor;
    SDL_Color m_HealthbarColor;
    SDL_Color m_HandColor;
    SDL_Color m_NameplateColor;
    SDL_Color m_HealthRed;
    SDL_Color m_HealthBlack;

    void TickKeyboardControls();
    void TickGameControllerControls();
    void TickControls();
    void TickHook();
    void TickCurrentWeapon();
    void DrawAmmo();

    void DrawCharacter();
    void DrawHook();
    void DrawHealthbar();
    void DrawHands();
    void DrawNameplate();

public:
    static Texture* ms_Texture;
    static Texture* ms_FistTexture;
    static Sound* ms_HitSounds[3];
    static Sound* ms_DeathSound;

    Character(GameWorld* world, double start_x, double start_y, double start_xvel, double start_yvel);
    ~Character();

    Hook* GetHook() { return &m_Hook; }
    GameController* GetGameController() const { return m_GameController; }
    int PlayerIndex() const { return m_PlayerIndex; }

    void SetGameController(GameController* gameController);
    void Accelerate(double accelerate_x, double accelerate_y);
    void Damage(double damage, bool make_sound);

    void Event(const SDL_Event& currentEvent);
    void Tick() override;
    void Draw() override;


};


#endif //TRIALANDERROR_CHARACTER_H
