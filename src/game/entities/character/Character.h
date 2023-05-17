#ifndef TRIALANDERROR_CHARACTER_H
#define TRIALANDERROR_CHARACTER_H

#include "SDL.h"
#include <cstring>
#include "../Entity.h"
#include "../../Player.h"
#include "../../weapons/WeaponGlock.h"
#include "../../weapons/WeaponShotgun.h"
#include "../../weapons/WeaponBurst.h"
#include "../../weapons/WeaponMinigun.h"
#include "../../../technical stuff/GameControllers.h"
#include "../../../technical stuff/Colors.h"
#include "../../indicators/HealthBar.h"
#include "../../indicators/TextSurface.h"
#include "../Ammo.h"
#include "Hook.h"
#include "Hands.h"

struct CharacterInput {
    bool m_Shooting;
    bool m_Reloading;
    bool m_Hooking;
    bool m_Sneaking;
    bool m_NextItem, m_PrevItem;
    double m_GoingX, m_GoingY;
    double m_LookingX, m_LookingY;

    CharacterInput();
};

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
    friend class Player;
    friend class GameWorld;
    friend class ProjectileWeapon;
    friend class Bullets; //
    friend class Hands;
    Player* m_Player;
    TextSurface* m_CoordinatePlate;
    TextSurface* m_AmmoCount;
    TextSurface* m_HealthInt;
    double m_ColorHue;
    int m_SelectedWeaponIndex;
    GameController* m_GameController;
    bool m_Movement[NUM_CONTROLS] {};
    bool m_NPC;
    CharacterInput m_Input, m_LastInput;
    // bool m_Using, m_LastUsing;
    bool IsReversed, ConfusingHP, Invincible, Spiky, HealersParadise, Ranged;
    Hands m_Hands;
    ProjectileWeapon* m_Weapons[NUM_WEAPONS];
    ProjectileWeapon* m_CurrentWeapon;
    double m_MaxHealth, m_Health, m_LastHealth;
    double m_ActiveRegeneration, m_PassiveRegeneration;
    unsigned long long m_TicksOfCombatUntilRegeneration;
    unsigned long long m_LastInCombat;
    static const int ms_DefaultControls[NUM_CONTROLS];
    double m_xLast, m_yLast;
    const double m_BaseAcceleration;
    double m_Acceleration;
    Hook m_Hook;
    int m_HitTicks;
    int m_Timer;
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
    void TickHealth();
    void TickControls();
    void TickHook();
    void TickCollision();
    void TickCurrentWeapon();
    void DrawAmmo();

    void DrawCharacter();
    void DrawHook();
    void DrawHealthbar();
    void DrawHands();
    void DrawNameplate();
    void TickTimer();

public:
    static Texture* ms_Texture;
    static Sound* ms_HitSounds[3];
    static Sound* ms_DeathSound;
    static Sound* ms_AmmoPickupSound;
    static TextSurface* ms_BotNamePlate;

    Character(GameWorld* world, Player* player, double max_health, double start_x, double start_y, double start_xvel, double start_yvel, bool bot_player);
    ~Character();

    Hook* GetHook() { return &m_Hook; }
    Player* GetPlayer() const { return m_Player; }
    GameController* GetGameController() const { return m_GameController; }
    bool IsNPC() const { return m_NPC; }
    ProjectileWeapon* GetCurrentWeapon() const { return m_CurrentWeapon; }
    CharacterInput& GetInput() { return m_Input; }
    CharacterInput& GetLastInput() { return m_LastInput; }

    void AmmoPickup(Ammo* ammo_box);
    void SetGameController(GameController* gameController);
    void Damage(double damage, bool make_sound);
    void SwitchWeapon(WeaponType type);
    void ReverseMovement();
    void ConfuseHP();
    void MakeInvincible();
    void MakeSpiky();
    void MakeHealer();
    void MakeRanged();

    void Event(const SDL_Event& currentEvent);
    void Tick() override;
    void Draw() override;
};


#endif //TRIALANDERROR_CHARACTER_H
