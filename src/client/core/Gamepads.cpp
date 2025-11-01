//
// Created by Matq on 20/03/2023.
//

#include "Gamepads.h"
#include <shared/utility/Strings.h>
#include <algorithm>

Gamepad::Gamepad()
{
	sdl_gamepad = nullptr;
	instance_id = -1;
	index = -1;

	metadata_name = "unknown";
	metadata_serial = "unknown";
	metadata_vendor = 0;
	metadata_product = 0;
	metadata_version = 0;
	metadata_type = SDL_GAMEPAD_TYPE_UNKNOWN;

	for (auto& one_axis : axis)
		one_axis = 0;
	for (bool& button : buttons)
		button = false;
}

Gamepad::~Gamepad()
{
	CloseGamepad();
}

Vec2f Gamepad::GetJoystickLeft()
{
	return { axis[SDL_GAMEPAD_AXIS_LEFTX], axis[SDL_GAMEPAD_AXIS_LEFTY] };
}

Vec2f Gamepad::GetJoystickRight()
{
	return { axis[SDL_GAMEPAD_AXIS_RIGHTX], axis[SDL_GAMEPAD_AXIS_RIGHTY] };
}

// Call this when detecting a new controller
void Gamepad::OpenGamepad(SDL_JoystickID open_instance_id)
{
	sdl_gamepad = SDL_OpenGamepad(open_instance_id);
	instance_id = open_instance_id;
	LoadMetadata();

	//
	for (auto& one_axis : axis)
		one_axis = 0;
	for (bool& button : buttons)
		button = false;
}

// Call this when adding existing controllers
void Gamepad::InitGamepad(SDL_Gamepad *init_sdl_gamepad, SDL_JoystickID init_instance_id)
{
	// This controller is already in use
	if (init_sdl_gamepad || instance_id != -1)
		return;

	sdl_gamepad = init_sdl_gamepad;
	instance_id = init_instance_id;
	LoadMetadata();

	//
	for (auto& one_axis : axis)
		one_axis = 0;
	for (bool& button : buttons)
		button = false;
}

void Gamepad::CloseGamepad()
{
	if (!sdl_gamepad)
		return;

	SDL_CloseGamepad(sdl_gamepad);
	sdl_gamepad = nullptr;
	instance_id = -1;
}

void Gamepad::LoadMetadata()
{
	// these crash for some reason, hmmmm

//	metadata_name = SDL_GetGamepadName(sdl_gamepad);
//	metadata_serial = SDL_GetGamepadSerial(sdl_gamepad); // crashes
//	metadata_vendor = SDL_GetGamepadVendor(sdl_gamepad);
//	metadata_product = SDL_GetGamepadProduct(sdl_gamepad);
//	metadata_version = SDL_GetGamepadProductVersion(sdl_gamepad);
//	metadata_type = SDL_GetGamepadType(sdl_gamepad);
}

bool Gamepad::TriggerVibrate(int low_frequency_rumble, int high_frequency_rumble, int duration_ms)
{
	return SDL_RumbleGamepad(sdl_gamepad, low_frequency_rumble, high_frequency_rumble, duration_ms);
}

void Gamepad::Event(const SDL_Event& event)
{
	switch (event.type)
	{
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		{
			bool Pressed = event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN;
			buttons[event.gbutton.button] = Pressed;
			break;
		}
		case SDL_EVENT_GAMEPAD_AXIS_MOTION:
		{
			int AxisID = event.gaxis.axis;
			axis[AxisID] = static_cast<float>(event.gaxis.value) / SDL_MAX_SINT16;
			break;
		}
	}
}

void Gamepad::TickLast()
{
	memcpy(&last_buttons, &buttons, sizeof(last_buttons));
}

Streamer<int, bool> GamepadsClass::sGamepadOneUpdated;
Streamer<int, bool> GamepadsClass::sGamepadTwoUpdated;
Streamer<int, bool> GamepadsClass::sGamepadThreeUpdated;
Streamer<int, bool> GamepadsClass::sGamepadFourUpdated;

void GamepadsClass::AddGamepad(SDL_JoystickID instance_id)
{
	// existing gamepad, return
	if (GetGamepad(instance_id))
		return;

	SDL_Gamepad *sdl_gamepad = SDL_GetGamepadFromID(instance_id);
	Gamepad *new_gamepad = new Gamepad();
	if (sdl_gamepad == nullptr)
	{
		new_gamepad->OpenGamepad(instance_id);
	}
	else
	{
		new_gamepad->InitGamepad(sdl_gamepad, instance_id);
	}

	int assign_index = 0;
	for (Gamepad* gamepad : gamepads)
	{
		if (gamepad->index != assign_index)
			break; // We found the next free index

		assign_index++;
	}

	new_gamepad->index = assign_index;
	gamepads.push_back(new_gamepad);
	dbg_msg("[Gamepads] Gamepad #%i has been connected, assigned index #%i\n", instance_id, assign_index);

	// let's call the event for corresponding controller
	bool added = true;
	if (assign_index == 0)
		sGamepadOneUpdated.Invoke(assign_index, added);
	else if (assign_index == 1)
		sGamepadTwoUpdated.Invoke(assign_index, added);
	else if (assign_index == 2)
		sGamepadThreeUpdated.Invoke(assign_index, added);
	else if (assign_index == 3)
		sGamepadFourUpdated.Invoke(assign_index, added);
	else
		dbg_msg("[Gamepads] Unhandled controller index #%i\n", assign_index);
}

Gamepad *GamepadsClass::RemoveGamepad(SDL_JoystickID instance_id)
{
	Gamepad *existing_gamepad = GetGamepad(instance_id);
	if (!existing_gamepad)
		return nullptr;

	int index = existing_gamepad->index;
	gamepads.erase(std::remove(gamepads.begin(), gamepads.end(), existing_gamepad), gamepads.end());
	dbg_msg("[Gamepads] Gamepad #%i has been disconnected, index #%i\n", instance_id, index);

	// let's call the event for corresponding controller
	bool added = false;
	if (index == 0)
		sGamepadOneUpdated.Invoke(index, added);
	else if (index == 1)
		sGamepadTwoUpdated.Invoke(index, added);
	else if (index == 2)
		sGamepadThreeUpdated.Invoke(index, added);
	else if (index == 3)
		sGamepadFourUpdated.Invoke(index, added);

	return existing_gamepad;
}

GamepadsClass::GamepadsClass()
{

}

GamepadsClass::~GamepadsClass()
{
	for (Gamepad *gamepad : gamepads)
		delete gamepad;
	gamepads.clear();
}

Gamepad *GamepadsClass::GetGamepad(SDL_JoystickID instance_id)
{
	SDL_Gamepad *sdl_gamepad = SDL_GetGamepadFromID(instance_id);
	if (!sdl_gamepad)
		return nullptr;

	for (Gamepad *gamepad : gamepads)
		if (gamepad->sdl_gamepad == sdl_gamepad)
			return gamepad;

	return nullptr;
}

Gamepad *GamepadsClass::GetGamepadFromIndex(int get_index)
{
	for (Gamepad *gamepad : gamepads)
		if (gamepad->index == get_index)
			return gamepad;

	return nullptr;
}

void GamepadsClass::AddMissingGamepads()
{
	int num_joysticks = 0;
	SDL_JoystickID *joystick_ids = SDL_GetGamepads(&num_joysticks);
	if (!joystick_ids)
		return;

	for (int i = 0; i < num_joysticks; i++)
		AddGamepad(joystick_ids[i]);
	SDL_free(joystick_ids);
}

void GamepadsClass::HandleEvent(const SDL_Event& sdl_event, EventContext& event_context)
{
	switch (sdl_event.type)
	{
		case SDL_EVENT_GAMEPAD_ADDED:
		{
			SDL_JoystickID instance_id = sdl_event.gdevice.which;
			AddGamepad(instance_id);
			break;
		}
		case SDL_EVENT_GAMEPAD_REMOVED:
		{
			Gamepad *removed_gamepad = RemoveGamepad(sdl_event.gdevice.which);
			removed_gamepad->CloseGamepad();
			delete removed_gamepad;
			break;
		}
	}

	for (Gamepad *gamepad : gamepads)
	{
		// Not our gamepad, continue
		if (gamepad->InstanceID() != sdl_event.gdevice.which)
			continue;

		gamepad->Event(sdl_event);
		break;
	}
}

void GamepadsClass::TickLast()
{
	for (Gamepad *gamepad : gamepads)
		gamepad->TickLast();
}

GamepadsClass Gamepads;