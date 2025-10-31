#pragma once

#include <client/game/entities/cartesian/characters/character/Hook.h>
#include <client/game/entities/cartesian/characters/character/Hands.h>
#include <client/game/weapons/projectile/WeaponShotgun.h>
#include <client/game/weapons/projectile/WeaponMinigun.h>
#include <client/game/weapons/projectile/WeaponGlock.h>
#include <client/game/weapons/projectile/WeaponBurst.h>
#include <client/game/entities/cartesian/AmmoBox.h>
#include <client/game/entities/cartesian/Entity.h>
#include <client/game/entities/cartesian/Crate.h>
#include <client/game/indicators/TextSurface.h>
#include <client/game/indicators/HealthBar.h>
#include <client/game/error/ErrorStatuses.h>
#include <client/core/GameControllers.h>
#include <client/game/players/Player.h>
#include <client/core/Assets.h>

#include <shared/utility/Colors.h>

#include <cstring>

struct CharacterInput
{
	bool shooting;
	bool reloading;
	bool hooking;
	bool sneaking;
	bool next_item, prev_item, deselect_item;
//	double m_GoingX, m_GoingY, m_GoingLength;
//	double m_LookingX, m_LookingY, m_LookingLength;

	Vec2f going_direction;
	Vec2f looking_direction;

	CharacterInput();
};

class Character : public DirectionalEntity
{
public:
	enum
	{
		CONTROL_UP,
		CONTROL_RIGHT,
		CONTROL_DOWN,
		CONTROL_LEFT,
		CONTROL_SNEAK,
		NUM_CONTROLS,
		CHARACTER_MAX_NAME_LENGTH = 32
	};

	protected:
		friend class Player;
		friend class GameWorld;
		friend class ProjectileWeapon;
		friend class Projectile; //
		friend class Hands;
		Player *m_Player;
//		Vec2d m_CameraTarget;
		TextSurface *m_CoordinatePlate;
		TextSurface *m_AmmoCount;
		TextSurface *m_HealthInt;
		double m_ColorHue;
		int m_SelectedWeaponIndex;
//		GameController *m_GameController;
		bool m_Movement[NUM_CONTROLS];
		bool m_NPC;
		CharacterInput input, m_LastInput;
		Hands m_Hands;
		ProjectileWeapon *m_Weapons[NUM_WEAPONS];
		ProjectileWeapon *m_CurrentWeapon;
		double m_ActiveRegeneration, m_PassiveRegeneration;
		unsigned long long m_TicksOfCombatUntilRegeneration;
		unsigned long long m_LastInCombat;
		static const int ms_DefaultControls[NUM_CONTROLS];

		const float m_BaseAcceleration;
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

		// Listening & Ticking
		virtual void EventDeath();
		virtual void TickKeyboardControls();
//		virtual void TickGameControllerControls();
		void TickHealth();
		virtual void TickControls();
		void TickProcessInputs();
		void TickHook(double elapsed_seconds);
		void TickCollision();
		void TickCurrentWeapon();
		void DrawAmmoCounter();
		void DrawErrorIcons();
		void DrawCharacter();
		void DrawHook();
		void DrawHealthbar();
		void DrawHands();
		virtual void DrawNameplate();
		void DrawErrorName();

public:
	static LinkTexture sCharacterTexture;
	static LinkTexture sTextureBlood;
	static LinkSound sHitSounds[3];
	static LinkSound sInvincibleHitSound;
	static LinkSound sDeathSound;
	static LinkSound sAmmoPickupSound;
	static LinkSound sItemSwitchSound;
	static LinkSound sThrowItemSound;
	static LinkSound sPickupItemSound;
	static TextSurface *ms_BotNamePlate;
	TextSurface *m_ErrorText;

	Character(GameWorld *world,
			  Player *player,
			  double max_health,
			  const Vec2f& start_pos,
			  const Vec2f& start_vel,
			  bool is_npc);
	~Character() override;

	// Getting
	[[nodiscard]] Hook *GetHook() { return &m_Hook; }
	[[nodiscard]] Player *GetPlayer() const { return m_Player; }
//	[[nodiscard]] GameController *GetGameController() const { return m_GameController; }
	[[nodiscard]] ProjectileWeapon *GetCurrentWeapon() const { return m_CurrentWeapon; }
	[[nodiscard]] CharacterInput& GetInput() { return input; }
	[[nodiscard]] CharacterInput& GetLastInput() { return m_LastInput; }
	[[nodiscard]] ErrorStatuses& GetErrorStatuses() { return m_ErrorStatuses; }
//	[[nodiscard]] Vec2d GetCameraTarget() const { return m_CameraTarget; }
	[[nodiscard]] bool IsNPC() const { return m_NPC; }
	[[nodiscard]] int GetBaseDamage() const { return m_Player ? m_Player->GetBaseDamage() : m_BaseDamage; }
	[[nodiscard]] double GetDamageAmp() const { return m_Player ? m_Player->GetDamageAmp() : m_DamageAmp; }

	// Setting
//	void SetGameController(GameController *game_controller) { m_GameController = game_controller; }
//	void SetCameraTarget(Vec2d cameraTarget) { m_CameraTarget = cameraTarget; }
	void RemoveCombat();
	void GiveWeapon(ProjectileWeapon *proj_weapon);
	void AmmoPickup(AmmoBox *ammo_box);
	void Damage(double damage, Entity *damager);
	void Heal(double value);
	void DropWeapon();
	void SwitchWeapon(WeaponType type);

	void LevelupStats(unsigned int level);

	// Listening & Ticking
	void HandleEvent(const SDL_Event& sdl_event);
	void Tick(double elapsed_seconds) override;
	void Draw() override;

};
