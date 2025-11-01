#pragma once

#include <client/game/entities/cartesian/characters/character/Hook.h>
#include <client/game/entities/cartesian/characters/character/Hands.h>
#include <client/game/weapons/projectile/base/ProjectileWeapon.h>
//#include <client/game/weapons/projectile/WeaponShotgun.h>
//#include <client/game/weapons/projectile/WeaponMinigun.h>
//#include <client/game/weapons/projectile/WeaponGlock.h>
//#include <client/game/weapons/projectile/WeaponBurst.h>
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
		Player *player;
//		Vec2d m_CameraTarget;
		TextSurface *coordinate_plate;
		TextSurface *ammo_count_plate;
		TextSurface *health_amount_plate;
		double color_hue;
		int selected_weapon_index;
//		GameController *m_GameController;
		bool movement[NUM_CONTROLS];
		bool is_npc;
		CharacterInput input, last_input;
		Hands hands;
		ProjectileWeapon *weapons[NUM_WEAPONS];
		ProjectileWeapon *current_weapon;
		double active_regeneration_rate, passive_regeneration_rate;
		unsigned long long ticks_combat_until_regeneration;
		unsigned long long last_combat_timestamp;
		static const int sDefaultControls[NUM_CONTROLS];

		const float base_acceleration;
		double damage_amplifier;
		Hook hook;
		int hit_ticks;
		int base_damage;
		HealthBar health_bar;
		ErrorStatuses error_statuses;

		SDL_Color character_color;
		SDL_Color hook_color;
		SDL_Color healthbar_color;
		SDL_Color hand_color;
		SDL_Color nameplate_color;
		SDL_Color health_red_color;
		SDL_Color health_black_color;

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
	static TextSurface *sBotNameplate;
	TextSurface *error_notification_text;

	Character(Player *player,
			  double max_health,
			  const Vec2f& start_pos,
			  const Vec2f& start_vel,
			  bool is_npc);
	~Character() override;

	// Getting
	[[nodiscard]] Hook *GetHook() { return &hook; }
	[[nodiscard]] Player *GetPlayer() const { return player; }
//	[[nodiscard]] GameController *GetGameController() const { return m_GameController; }
	[[nodiscard]] ProjectileWeapon *GetCurrentWeapon() const { return current_weapon; }
	[[nodiscard]] CharacterInput& GetInput() { return input; }
	[[nodiscard]] CharacterInput& GetLastInput() { return last_input; }
	[[nodiscard]] ErrorStatuses& GetErrorStatuses() { return error_statuses; }
//	[[nodiscard]] Vec2d GetCameraTarget() const { return m_CameraTarget; }
	[[nodiscard]] bool IsNPC() const { return is_npc; }
	[[nodiscard]] int GetBaseDamage() const { return player ? player->GetBaseDamage() : base_damage; }
	[[nodiscard]] double GetDamageAmp() const { return player ? player->GetDamageAmp() : damage_amplifier; }

	// Setting
//	void SetGameController(GameController *game_controller) { m_GameController = game_controller; }
//	void SetCameraTarget(Vec2d cameraTarget) { m_CameraTarget = cameraTarget; }
	void RemoveCombat();
	void GiveWeapon(ProjectileWeapon *new_weapon);
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
