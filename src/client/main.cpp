/*
 *  Trial And Error
 *  This is a game.
 *
 *  Made by two and a half guys.
 *  Project started 16.03.23
 */

#define SDL_MAIN_HANDLED

#ifdef _WIN32
#include <windows.h>
#undef interface
#undef PlaySound
#undef ERROR
#endif

#include <client/game/entities/cartesian/characters/CharacterNPC.h>
#include <client/game/entities/cartesian/Projectile.h>
#include <client/game/entities/cartesian/AmmoBox.h>
#include <client/game/ui/menus/Menus.h>
#include <client/game/GameReference.h>
#include <client/utility/Debugging.h>
#include <client/core/TextManager.h>
#include <client/Protocol.h>

#include <SDL3/SDL_main.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>

LinkTexture sPlanetTexture("backgrounds.planet2");
LinkSound sConnectedSound("ui.pitch.mid");
LinkSound sDisconnectedSound("ui.pitch.low");

void SetConsoleToUTF8()
{
	SetConsoleOutputCP(CP_UTF8);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hOut, &mode);
	SetConsoleMode(hOut, mode | ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
}

void exit_application()
{
	GameReference.Deinitialize(true);
	Application.Destroy();
	dbg_msg("Graceful exit\n");
	exit(0);
}

// Generate points uniformly distributed on a sphere
std::vector<Vec3f> GenerateSpherePoints(float radius, int numPoints)
{
	std::vector<Vec3f> points;
	points.reserve(numPoints);

	// Using Fibonacci sphere algorithm for uniform distribution
	float goldenRatio = (1.0f + std::sqrt(5.0f)) / 2.0f;
	float angleIncrement = 2.0f * M_PI * goldenRatio;

	for (int i = 0; i < numPoints; i++)
	{
		float t = static_cast<float>(i) / static_cast<float>(numPoints);
		float inclination = std::acos(1.0f - 2.0f * t);
		float azimuth = angleIncrement * i;

		float x = radius * std::sin(inclination) * std::cos(azimuth);
		float y = radius * std::sin(inclination) * std::sin(azimuth);
		float z = radius * std::cos(inclination);

		points.push_back(Vec3f(x, y, z));
	}

	return points;
}

int main()
{
#ifdef NDEBUG
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

	system("");
	SetConsoleToUTF8();

	srand(time(nullptr));
	GameReference.SetExitApplicationCallback([]() { exit_application(); });
	Application.Initialize(APPLICATION_NAME,
						   APPLICATION_VERSION,
						   "com.tae.trial_and_error",
						   Vec2i(1280, 720),
						   60.0, 10.0);

	auto clock = Application.GetClock();
	auto drawing = Application.GetDrawing();

	long long total_frame_duration = 1;
	SDL_Event sdl_event;
	EventContext event_context;
	do // Loading loop
	{
		event_context.ResetContext(); // Per tick context
		// Events
		while (SDL_PollEvent(&sdl_event))
		{
			event_context.ResetRapidContext(); // Per event context
			Application.HandleEvent(sdl_event, event_context);
			switch (sdl_event.type)
			{
				case SDL_EVENT_QUIT:
				{
					exit_application();
					break;
				}
			}
		}

		// Ticking
		Assets.ThreadedLoading();

		// Drawing
		if (clock->TimePassed())
		{
			drawing->SetRenderTarget(nullptr);
			drawing->SetColor(0, 0, 0, 255);
			drawing->Clear();

			// Draw progress bar
			SDL_FRect progress_rect = {
				((float)Application.GetWidth() - 800.0f) / 2.0f,
				((float)Application.GetHeight() - 50.0f) / 2.0f,
				800.0f * ((float)AssetsClass::GetWorkDone() / (float)AssetsClass::GetTotalWork()),
				50.0f
			};
			drawing->SetColor(104, 195, 235, 255);
			drawing->DrawRect(progress_rect, true);

			progress_rect.w = 800.0f;
			progress_rect.h = 50.0f;
			drawing->DrawRect(progress_rect, false);

			drawing->UpdateWindow();
		}
	} while (Assets.IsLoading());

	Gamepads.AddMissingGamepads();
	GameReference.Initialize();

	// Do this after assets have been loaded because it uses them
	Menus.InitMenus();
	Menus.main_menu->SwitchToThisMenu();

	Texture *Vignette = Assets.GetTexture("backgrounds.vignette");
	if (Vignette)
		Vignette->SetAlphaMod(200);

	MainMenu mainMenu;
	mainMenu.SwitchToThisMenu();
//	GameReference.Get
//	mainMenu.InitialShow();

//	PauseMenu *PauseMenu;

	Vec3f cubes_at = Vec3f(0.0f, 0.0f, 100.0f);
	std::vector<Mesh> shapes;
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
			{
				Mesh new_cube = MeshPresets::CreateCube(10.0f);
				new_cube.position = Vec3f(
					-15.0f + x * 15.0f,
					-15.0f + y * 15.0f,
					-15.0f + z * 15.f
				) + cubes_at;
				shapes.push_back(new_cube);
			}

	Mesh sphere = MeshPresets::CreateSphere(30.0f, 30, 50);
	sphere.position = Vec3f(70.0f, 0.0f, 0.0f) + cubes_at;
	shapes.push_back(sphere);

//	Mesh ground = MeshPresets::CreateGround(1000.0f, 100);
//	shapes.push_back(ground);

	Scene scene;
	DirectionLight direction_light(Vec3f(-0.5f, 1.0f, -0.5f), Vec3f(1.0f, 1.0f, 1.0f), 0.5f);

//	Mesh& ground_mesh = shapes.back(); // safe while vector is not reallocated

	while (true)
	{
		// This part of the loop runs with no delay
		event_context.ResetContext(); // Per tick context

		// Input and events
		while (SDL_PollEvent(&sdl_event))
		{
			event_context.ResetRapidContext(); // Per event context

			Gamepads.HandleEvent(sdl_event, event_context);
			Application.HandleEvent(sdl_event, event_context);
			Menus.HandleEvent(sdl_event, event_context);
			GameReference.HandleEvent(sdl_event, event_context);

			switch (sdl_event.type)
			{
				case SDL_EVENT_QUIT:
				{
					exit_application();
					break;
				}
				case SDL_EVENT_MOUSE_MOTION:
				{
					event_context.had_mouse_motion = true;
					break;
				}
				case SDL_EVENT_WINDOW_MOUSE_LEAVE:
				{
					auto window_flags = SDL_GetWindowFlags(Application.GetWindow());
					if ((window_flags & SDL_WINDOW_INPUT_FOCUS) == 0)
						clock->SetIdleFramerate();

					break;
				}
				case SDL_EVENT_WINDOW_MOUSE_ENTER:
				{
					auto window_flags = SDL_GetWindowFlags(Application.GetWindow());
					if ((window_flags & SDL_WINDOW_INPUT_FOCUS) == 0)
						clock->ResetFramerate();

					break;
				}
				case SDL_EVENT_WINDOW_FOCUS_LOST:
				{
					clock->SetIdleFramerate();
					break;
				}
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
				{
					clock->ResetFramerate();
					break;
				}
				case SDL_EVENT_KEY_DOWN:
				{

					break;
				}
			}
			Element::DestroyElements();
		}
		if (event_context.had_mouse_motion && event_context.cursor_changed == CursorChange::NO_CHANGE)
			SDL_SetCursor(SDL_GetDefaultCursor());

		// This part of the loop runs at 60fps
		if (clock->TimePassed())
		{
			auto elapsed_seconds = (double)(total_frame_duration) / 1e9;

			// Ticking
			if (GameReference.World())
				GameReference.World()->Tick(elapsed_seconds);
			Menus.Tick(elapsed_seconds);
			Gamepads.TickLast();

			// Drawing
			if (GameReference.World())
				GameReference.World()->Draw();
			GameReference.GetInterface()->DrawBackground();

			{ // 3d stuff
				Camera3D& camera = GameReference.GetCamera3D();

				// move 3d camera controller
				if (!Gamepads.GetGamepads().empty())
				{
					Gamepad *gamepad = Gamepads.GetGamepads().front();
					Vec2f left_joystick = gamepad->GetJoystickLeft();
					Vec2f right_joystick = gamepad->GetJoystickRight();
					float triggers = gamepad->GetLeftTrigger() - gamepad->GetRightTrigger();

					// Movement (using left stick)
					float move_speed = 50.0f;
					bool go_up = gamepad->GetButton(SDL_GAMEPAD_BUTTON_DPAD_UP);
					bool go_down = gamepad->GetButton(SDL_GAMEPAD_BUTTON_DPAD_DOWN);
					float forward = -left_joystick.y * move_speed * elapsed_seconds;
					float strafe = left_joystick.x * move_speed * elapsed_seconds;
					float vertical = static_cast<float>(-go_down + go_up) * move_speed * elapsed_seconds;
					camera.MoveRelative(forward, strafe, vertical);

					// Rotation (using right stick)
					float look_sensitivity = 2.0f;
					if (right_joystick.x != 0.0f || right_joystick.y != 0.0f || triggers != 0.0f)
					{
						float yaw = right_joystick.x * look_sensitivity * elapsed_seconds;
						float pitch = right_joystick.y * look_sensitivity * elapsed_seconds;
						float roll = triggers * look_sensitivity * elapsed_seconds;

						// Get current local axes
						Vec3f up_axis = camera.GetUp();
						Vec3f right_axis = camera.GetRight();
						Vec3f forward_axis = camera.GetLook();

						// Create rotation quaternions around LOCAL axes
						Quaternion yaw_rot = Quaternion::FromAxisAngle(up_axis, yaw);
						Quaternion pitch_rot = Quaternion::FromAxisAngle(right_axis, pitch);
						Quaternion roll_rot = Quaternion::FromAxisAngle(forward_axis, roll);

						// Apply rotations: BOTH from the left
						Quaternion current = camera.GetRotation();
						Quaternion new_rotation = (roll_rot * pitch_rot * yaw_rot * current).Normalize();
						camera.SetRotation(new_rotation);
					}

				}

//				drawing->SetColor(200, 200, 200, 255);
//				drawing->Clear();

//				ground_mesh.position = Vec3f(camera.GetPosition().x, 0.0f, camera.GetPosition().z);

				scene.Clear();
				for (const Mesh& shape : shapes)
					scene.ProjectMesh(shape, camera, &direction_light);
				scene.SortAndCommit();
//				drawing->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
				drawing->RenderScene(scene, sPlanetTexture);
//				drawing->SetDrawBlendMode(SDL_BLENDMODE_NONE);

				// wireframe
//				drawing->SetColor(255, 255, 255, 50);
//				drawing->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
//				for (const Mesh& cube : shapes)
//					drawing->RenderMeshWireframe(cube, camera);
//				drawing->SetDrawBlendMode(SDL_BLENDMODE_NONE);
			}

			if (Vignette)
				drawing->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

			Menus.Render();

			// Render one of the levelupmenus in queue if any
//			if (GameReference.World()->LvlMenu())
//				GameReference.World()->LvlMenu()->Render();
//			else
//				GameReference.World()->CheckLevelUps();

			GameReference.GetInterface()->DrawForeground();
			drawing->UpdateWindow();

//			if (GameReference.World()->GetDelay() && (GameReference.World()->LvlMenu() != nullptr))
//				GameReference.World()->SetDelay(false); // Reset the delay flag after the delay
		}

		total_frame_duration = clock->GetTimeElapsedNanoNow();
	}

	exit_application();
	return 0;
}
