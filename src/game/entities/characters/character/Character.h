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
#include "../../../interface/LevelUpMenu.h"
#include "../../../indicators/TextSurface.h"
#include "../../AmmoBox.h"
#include "../../Crate.h"
#include "Hook.h"
#include "Hands.h"
#include "../../../../client/Assets.h"
#include "../../../error/ErrorStatuses.h"

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
    TextSurface* m_CoordinatePlate;
    TextSurface* m_AmmoCount;
    TextSurface* m_HealthInt;
    double m_ColorHue;
    int m_SelectedWeaponIndex;
    GameController* m_GameController;
    bool m_Movement[NUM_CONTROLS];
    bool m_NPC;
    CharacterInput m_Input, m_LastInput;
    Hands m_Hands;
    ProjectileWeapon* m_Weapons[NUM_WEAPONS];
    ProjectileWeapon* m_CurrentWeapon;
    double m_ActiveRegeneration, m_PassiveRegeneration;
    unsigned long long m_TicksOfCombatUntilRegeneration;
    unsigned long long m_LastInCombat;
    static const int ms_DefaultControls[NUM_CONTROLS];

    const double m_BaseAcceleration;
    double m_DamageAmp;
    Hook m_Hook;
    int m_HitTicks;
    int m_BaseDamage;
    HealthBar m_HealthBar;
    ErrorStatuses m_ErrorStatuses;

    SDL_Color m_CharacterColor;
    SDL_Color m_HookColor;
    SDL_Color m_HealthbarColor;
    SDL_Color m_HandColor;
    SDL_Color m_NameplateColor;
    SDL_Color m_HealthRed;
    SDL_Color m_HealthBlack;

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
    void DrawAmmoCounter();
    void DrawErrorIcons();
    void DrawCharacter();
    void DrawHook();
    void DrawHealthbar();
    void DrawHands();
    void DrawNameplate();
    void DrawErrorName();

public:
    static LoadedTexture sCharacterTexture;
    static LoadedTexture sTextureGlock;
    static LoadedTexture sTextureShotgun;
    static LoadedTexture sTextureBurst;
    static LoadedTexture sTextureSniper;
    static LoadedTexture sTexturePatersonNavy;
    static LoadedTexture sTexturesMinigun[4];
    static LoadedTexture sTextureBlood;
    static LoadedSound sHitSounds[3];
    static LoadedSound sInvincibleHitSound;
    static LoadedSound sDeathSound;
    static LoadedSound sAmmoPickupSound;
    static LoadedSound sItemSwitchSound;
    static LoadedSound sThrowItemSound;
    static LoadedSound sPickupItemSound;
    static TextSurface* ms_BotNamePlate;
    TextSurface* m_ErrorText;

    Character(GameWorld* world,
              Player* player,
              double max_health,
              const Vec2d& start_pos,
              const Vec2d& start_vel);
    ~Character();

    // Getting
    [[nodiscard]] Hook* GetHook() { return &m_Hook; }
    [[nodiscard]] Player* GetPlayer() const { return m_Player; }
    [[nodiscard]] GameController* GetGameController() const { return m_GameController; }
    [[nodiscard]] ProjectileWeapon* GetCurrentWeapon() const { return m_CurrentWeapon; }
    [[nodiscard]] CharacterInput& GetInput() { return m_Input; }
    [[nodiscard]] CharacterInput& GetLastInput() { return m_LastInput; }
    [[nodiscard]] ErrorStatuses& GetErrorStatuses() { return m_ErrorStatuses; }
    [[nodiscard]] bool IsNPC() const { return m_NPC; }
    [[nodiscard]] int GetBaseDamage() const { if (m_Player)return m_Player->GetBaseDamage(); else return m_BaseDamage; }
    [[nodiscard]] double GetDamageAmp() const { if (m_Player)return m_Player->GetDamageAmp(); else return m_DamageAmp; }

    // Setting
    void SetGameController(GameController* game_controller) { m_GameController = game_controller; }
    void RemoveCombat();
    void GiveWeapon(ProjectileWeapon* proj_weapon);
    void AmmoPickup(AmmoBox* ammo_box);
    void Damage(double damage, Entity* damager);
    void Heal(double value);
    void DropWeapon();
    void SwitchWeapon(WeaponType type);

    void LevelupStats(unsigned int level);

    // Listening & Ticking
    void Event(const SDL_Event& currentEvent);
    void Tick() override;
    void Draw() override;

};
