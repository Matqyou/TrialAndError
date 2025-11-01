//
// Created by Matq on 19/12/2024.
//

#include "client/game/ui/cursors/Cursors.h"
#include "Application.h"

void ApplicationClass::PrintVersions()
{
	std::cout << Strings::FStringColors("&8SDL %d.%d.%d\n",
										SDL_MAJOR_VERSION,
										SDL_MINOR_VERSION,
										SDL_MICRO_VERSION);
	std::cout << Strings::FStringColors("&8SDLimage %u.%u.%u\n",
										SDL_IMAGE_MAJOR_VERSION,
										SDL_IMAGE_MINOR_VERSION,
										SDL_IMAGE_MICRO_VERSION);
	std::cout << Strings::FStringColors("&8SDLmixer %u.%u.%u\n",
										SDL_MIXER_MAJOR_VERSION,
										SDL_MIXER_MINOR_VERSION,
										SDL_MIXER_MICRO_VERSION);
	std::cout << Strings::FStringColors("&8SDLttf %u.%u.%u\n",
										SDL_TTF_MAJOR_VERSION,
										SDL_TTF_MINOR_VERSION,
										SDL_TTF_MICRO_VERSION);
}

ApplicationClass::ApplicationClass()
{
	window = nullptr;
	renderer = nullptr;
	drawing = nullptr;
	randomizer = nullptr;
	status = Status::UNINITIALIZED;
}

void ApplicationClass::Initialize(const char *title,
								  const char *version,
								  const char *identifier,
								  const Vec2i& window_resolution,
								  double framerate, double idle_framerate,
								  const char *renderer_backend)
{
	if (status == Status::INITIALIZED)
		return;
	status = Status::INITIALIZED;

	clock.SetNewBaseFramerate(framerate);
	clock.SetNewIdleFramerate(idle_framerate);

	system("");
	Strings::PrintDivider(title);

	dbg_msg("&3%s %s\n", title, version);

	bool metadata = SDL_SetAppMetadata(title, version, identifier);
	if (!metadata)
	{
		dbg_msg("[Application] &cCould not set SDL Metadata\n");
		dbg_msg("[Application] &cReason: %s\n", SDL_GetError());
	}

	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

	bool init_sdl = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD);
	if (!init_sdl)
		throw std::runtime_error(Strings::FString("Error while initializing sdl %s\n", SDL_GetError()));

	bool init_ttf = TTF_Init();
	if (!init_ttf)
		throw std::runtime_error(Strings::FString("Error while initializing TTF %s\n", SDL_GetError()));

	SDL_AudioSpec desired_spec;
	SDL_zero(desired_spec);
	desired_spec.freq = 44100;
	desired_spec.format = SDL_AUDIO_S16;
	desired_spec.channels = 2;

	SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired_spec);
	if (device_id == 0)
	{
		dbg_msg("[Application] &cCould not open audio device\n");
		dbg_msg("[Application] &cReason: %s\n", SDL_GetError());
		return; // or handle error appropriately
	}

	if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0)
		throw std::runtime_error(Strings::FString("Error while initializing Mix %s\n", SDL_GetError()));

	// Pass the device_id and spec to Mix_OpenAudio
	if (!Mix_OpenAudio(device_id, &desired_spec))
	{
		dbg_msg("[Application] &cCould not open Mix audio\n");
		dbg_msg("[Application] &cReason: %s\n", SDL_GetError());
		SDL_CloseAudioDevice(device_id);
		return; // or handle error appropriately
	}

	PrintVersions();

	resolution = window_resolution;
	half_resolution = Vec2f(window_resolution) / 2.f;
	window = SDL_CreateWindow(title, window_resolution.x, window_resolution.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (!window)
		throw std::runtime_error(Strings::FString("Error while creating the window %s\n", SDL_GetError()));

	std::cout << Strings::FStringColors("[Application] &6Available render back-ends:\n");
	int numDrivers = SDL_GetNumRenderDrivers();
	for (int i = 0; i < numDrivers; i++)
	{
		auto info = SDL_GetRenderDriver(i);
		std::cout << Strings::FStringColors("[Application] &6#%i %s\n", i, info);
	}

	renderer = SDL_CreateRenderer(window, renderer_backend != nullptr ? renderer_backend : "direct3d11");
	if (!renderer)
		throw std::runtime_error(Strings::FString("Error while creating the renderer %s\n", SDL_GetError()));

	std::cout << Strings::FStringColors("[Application] &eUsed renderer: %s\n", SDL_GetRendererName(renderer));

	drawing = new Drawing(renderer);
	randomizer = new Randomizer();

	Cursors::Initialize();
	Assets.Initialize(true); //

	Strings::PrintDivider();
}

void ApplicationClass::Destroy()
{
	if (status == Status::UNINITIALIZED)
		return;
	status = Status::UNINITIALIZED;

	Strings::PrintDivider("Destroying");

	Cursors::Deinitialize();
	Assets.Destroy();
	delete drawing;

	// SDL objects
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	// SDL as a whole
	TTF_Quit();
//    IMG_Quit();
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();

	Strings::PrintDivider();
}

ApplicationClass::~ApplicationClass()
{
	Destroy();
}

Vec2f ApplicationClass::GetMousePosition()
{
	Vec2f position;
	SDL_GetMouseState(&position.x, &position.y);

	return position;
}

void ApplicationClass::HandleEvent(const SDL_Event& sdl_event, EventContext& event_context)
{
	switch (sdl_event.type)
	{
		case SDL_EVENT_WINDOW_RESIZED:
		{
			resolution.x = sdl_event.window.data1;
			resolution.y = sdl_event.window.data2;
			half_resolution = Vec2f(resolution) / 2.f;
			break;
		}
//		case SDL_EVENT_GAMEPAD_ADDED:
//		{
//			dbg_msg("Controller added, Event: %i\n", sdl_event.type);
//			break;
//		}
	}
}

ApplicationClass Application;