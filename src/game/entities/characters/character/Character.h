#pragma once

#include "SDL.h"
#include <cstring>
#include "../../Entity.h"
#include "../../../Player.h"
#include "../../../weapons/projectile/WeaponGlock.h"
#include "../../../weapons/projectile/WeaponShotgun.h"
#include "../../../weapons/projectile/WeaponBurst.h"
#include "../../../weapons/projectile/WeaponMinigun.h"
#include "../../../../technical stuff/GameControllers.h"
#include "../../../../technical stuff/Colors.h"
#include "../../../indicators/HealthBar.h"
#include "../../../indicators/TextSurface.h"
#include "../../AmmoBox.h"
#include "../../Crate.h"
#include "Hook.h"
#include "Hands.h"
#include "../../../../client/Decals.h"

struct CharacterInput {
    bool m_Shooting;
    bool m_Reloading;
    bool m_Hooking;
    bool m_Sneaking;
    bool m_NextItem, m_PrevItem, m_DeselectItem;
    double m_GoingX, m_GoingY, m_GoingLength;
    double m_LookingX, m_LookingY, m_LookingLength;

    CharacterInput();
};

class Character : public DirectionalEntity {
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

protected:
    friend class Player;
    friend class GameWorld;
    friend class ProjectileWeapon;
    friend class Projectile; //
    friend class Hands;
    Player* m_Player;
    Texture2* m_Texture;
    TextSurface* m_CoordinatePlate;
    TextSurface* m_AmmoCount;
    TextSurface* m_HealthInt;
    double m_ColorHue;
    int m_SelectedWeaponIndex;
    GameController* m_GameController;
    bool m_Movement[NUM_CONTROLS]{ };
    bool m_NPC;
    CharacterInput m_Input, m_LastInput;
    bool IsReversed, ConfusingHP, Invincible, Spiky, HealersParadise, Ranged, IsSlow, m_DangerousRecoil;
    bool ReverseMSG, ConfusingHPMSG, InvincibleMSG, SpikyMSG, HealersMSG, RangedMSG, IsSlowMSG, RecoilMSG;
    Hands m_Hands;
    ProjectileWeapon* m_Weapons[NUM_WEAPONS];
    ProjectileWeapon* m_CurrentWeapon;
    double m_ActiveRegeneration, m_PassiveRegeneration;
    unsigned long long m_TicksOfCombatUntilRegeneration;
    unsigned long long m_LastInCombat;
    static const int ms_DefaultControls[NUM_CONTROLS];

    const double m_BaseAcceleration;
    Hook m_Hook;
    int m_HitTicks;
    int m_IsReverseTimer, m_ConfusingHPTimer, m_InvincibleTimer, m_SpikyTimer, m_HealersParadiseTimer, m_RangedTimer;
    int m_IsSlowTimer, m_DangerousRecoilTimer;
    float Displacement;
    HealthBar m_HealthBar;

    SDL_Color m_CharacterColor;
    SDL_Color m_HookColor;
    SDL_Color m_HealthbarColor;
    SDL_Color m_HandColor;
    SDL_Color m_NameplateColor;
    SDL_Color m_HealthRed; // TODO: why
    SDL_Color m_HealthBlack;

    SDL_FRect DrawErrorIsReversed, DrawErrorConfusingHP, DrawErrorInvincible, DrawErrorSpiky, DrawErrorHealersParadise;
    SDL_FRect DrawErrorRanged, DrawErrorIsSlow, DrawErrorDangerousRecoil;
    // This is how i did le spawning of icons to be displaced, big brain me yes yes
    // TODO: Texture instance class with SDL_FRect or SDL_Rect

    // Listening & Ticking
    virtual void EventDeath();
    void TickKeyboardControls();
    void TickGameControllerControls();
    void TickHealth();
    virtual void TickControls();
    void TickProcessInputs();
    void TickHook();
    void TickCollision();
    void TickCurrentWeapon();
    void TickErrorTimers();
    void DrawAmmoCounter();
    void DrawErrorIcons();
    void DrawCharacter();
    void DrawHook();
    void DrawHealthbar();
    void DrawHands();
    void DrawNameplate();
    void DrawErrorName();

public:
//    static Texture* ms_Texture;
    static Texture* ms_TextureGlock;
    static Texture* ms_TextureShotgun;
    static Texture* ms_TextureBurst;
    static Texture* ms_TextureSniper;
    static Texture* ms_TexturesMinigun[4];
    static Texture* ms_TextureErrorDisorianted;
    static Texture* ms_TextureErrorSpiky;
    static Texture* ms_TextureErrorConfusingHP;
    static Texture* ms_TextureErrorInvincible;
    static Texture* ms_TextureErrorHealersParadise;
    static Texture* ms_TextureErrorRanged;
    static Texture* ms_TextureErrorSlowDown;
    static Texture* ms_TextureErrorDangerousRecoil;
    static Texture* ms_TextureError;
    static Sound* ms_HitSounds[3];
    static Sound* ms_DeathSound;
    static Sound* ms_AmmoPickupSound;
    static Sound* ms_ItemSwitchSound;
    static TextSurface* ms_BotNamePlate;
    TextSurface* m_ErrorText;

    Character(GameWorld* world,
              Player* player,
              double max_health,
              const Vec2d& start_pos,
              const Vec2d& start_vel);
    ~Character() override;

    // Getting
    [[nodiscard]] Hook* GetHook() { return &m_Hook; }
    [[nodiscard]] Player* GetPlayer() const { return m_Player; }
    [[nodiscard]] GameController* GetGameController() const { return m_GameController; }
    [[nodiscard]] ProjectileWeapon* GetCurrentWeapon() const { return m_CurrentWeapon; }
    [[nodiscard]] CharacterInput& GetInput() { return m_Input; }
    [[nodiscard]] CharacterInput& GetLastInput() { return m_LastInput; }
    [[nodiscard]] bool IsNPC() const { return m_NPC; }
    [[nodiscard]] bool HasDangerousRecoil() const { return m_DangerousRecoil; }

    // Setting
    void SetGameController(GameController* game_controller) { m_GameController = game_controller; }
    void RemoveCombat();
    void GiveWeapon(ProjectileWeapon* proj_weapon);
    void AmmoPickup(AmmoBox* ammo_box);
    void Damage(double damage, Entity* damager) ;
    void Heal(double value) ;
    void DropWeapon();
    void SwitchWeapon(WeaponType type);
    void ReverseMovement();
    void ConfuseHP();
    void MakeInvincible();
    void MakeSpiky();
    void MakeHealer();
    void MakeRanged();
    void SlowDown();
    void ActivateDangerousRecoil();

    // Listening & Ticking
    void Event(const SDL_Event& currentEvent);
    void Tick() override;
    void Draw() override;
};
