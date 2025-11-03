//
// Created by Matq on 20/03/2023.
//

#pragma once

#include "client/game/ui/EventContext.h"
#include "shared/code/Streamer.h"
#include "shared/math/Vec2.h"
#include "client/core/Assets.h"

#include <SDL3/SDL.h>
#include <string>
#include <vector>

class GamepadsClass;
class Gamepad
{
private:
	friend class GamepadsClass;
	SDL_Gamepad *sdl_gamepad;
	SDL_JoystickID instance_id;
	int index; // actual player number (player1, player2, etc.)

	// metadata
	std::string metadata_name;
	std::string metadata_serial;
	Uint16 metadata_vendor;
	Uint16 metadata_product;
	Uint16 metadata_version;
	SDL_GamepadType metadata_type;

	float axis[SDL_GAMEPAD_AXIS_COUNT];
	bool buttons[SDL_GAMEPAD_BUTTON_COUNT];
	bool last_buttons[SDL_GAMEPAD_BUTTON_COUNT];

	Texture* visual_of_joystick_left;
	Texture* visual_of_joystick_right;

public:
	Gamepad();
	~Gamepad();

	// Getting
	[[nodiscard]] SDL_Gamepad *GetSDLGamePad() const { return sdl_gamepad; }
	[[nodiscard]] SDL_JoystickID InstanceID() const { return instance_id; }
	[[nodiscard]] bool IsOpen() const { return sdl_gamepad != nullptr; }
	[[nodiscard]] float GetLeftTrigger() const { return axis[SDL_GAMEPAD_AXIS_LEFT_TRIGGER]; }
	[[nodiscard]] float GetRightTrigger() const { return axis[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER]; }
	[[nodiscard]] bool GetButton(int button) const { return buttons[button]; }
	[[nodiscard]] bool GetLastButton(int button) const { return last_buttons[button]; }

	// Generating
	[[nodiscard]] Vec2f GetJoystickLeft();
	[[nodiscard]] Vec2f GetJoystickRight();

	// Manipulate
	void OpenGamepad(SDL_JoystickID open_instance_id);
	void InitGamepad(SDL_Gamepad *init_sdl_gamepad, SDL_JoystickID init_instance_id);
	void CloseGamepad();
	void LoadMetadata();
	bool TriggerVibrate(int low_frequency_rumble, int high_frequency_rumble, int duration_ms);

	// Listening & Ticking
	void Event(const SDL_Event& event);
	void TickLast();

};

class GamepadsClass
{
private:
	std::vector<Gamepad *> gamepads;

	void AddGamepad(SDL_JoystickID instance_id);
	Gamepad* RemoveGamepad(SDL_JoystickID instance_id);

public:
	// allow to connect to 4 controllers at the moment
	//  index, added/removed
	static Streamer<int, bool> sGamepadOneUpdated;
	static Streamer<int, bool> sGamepadTwoUpdated;
	static Streamer<int, bool> sGamepadThreeUpdated;
	static Streamer<int, bool> sGamepadFourUpdated;

	GamepadsClass();
	~GamepadsClass();

	// Getting
	[[nodiscard]] std::vector<Gamepad *>& GetGamepads() { return gamepads; }
	[[nodiscard]] Gamepad *GetGamepad(SDL_JoystickID instance_id);
	[[nodiscard]] Gamepad *GetGamepadFromIndex(int get_index);

	// Manipulating
	void AddMissingGamepads();

	// Listening & Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_context);
	void TickLast();

};

extern GamepadsClass Gamepads;