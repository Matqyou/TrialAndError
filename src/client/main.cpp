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
#include <client/Protocol.h>

#include <SDL3/SDL_main.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>

LinkSound sConnectedSound("ui.pitch.mid");
LinkSound sDisconnectedSound("ui.pitch.low");

LoadTexture sPlanetTexture("backgrounds.planet2", AssetsClass::TexturePurpose::OBJECT_3D);
LoadTexture sTest3DTexture("entity.character.body", AssetsClass::TexturePurpose::OBJECT_3D);

LoadTexture sVignette("backgrounds.vignette", AssetsClass::TexturePurpose::GUI_ELEMENT);

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

int main()
{
#ifdef NDEBUG
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

	system("");
	SetConsoleToUTF8();

	srand(time(nullptr));
	GameReference.SetExitApplicationCallback([]() { exit_application(); });
	Application.Initialize(
		APPLICATION_NAME,
		APPLICATION_VERSION,
		"com.tae.trial_and_error",
		Vec2i(1280, 720),
		60.0, 10.0
	);

	auto clock = Application.GetClock();

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
			// Draw progress bar
			SDL_FRect inner_rect = {
				((float)Application.GetWidth() - 800.0f) / 2.0f,
				((float)Application.GetHeight() - 50.0f) / 2.0f,
				800.0f, 50.0f,
			};
//			Drawing.DrawFilledRect(inner_rect, 255, 195, 235, 255);

			SDL_FRect progress_rect = {
				((float)Application.GetWidth() - 800.0f) / 2.0f,
				((float)Application.GetHeight() - 50.0f) / 2.0f,
				800.0f * ((float)AssetsClass::GetWorkDone() / (float)AssetsClass::GetTotalWork()),
				50.0f
			};
//			Drawing.DrawFilledRect(progress_rect, 104, 195, 235, 255);

//			Drawing.UpdateRender();




//			Drawing.SetRenderTarget(nullptr);
//			Drawing.SetColor(0, 0, 0, 255);
//			Drawing.Clear();
//
//
//			Drawing.SetColor(104, 195, 235, 255);
//			Drawing.DrawRect(progress_rect, true);
//
//			progress_rect.w = 800.0f;
//			progress_rect.h = 50.0f;
//			Drawing.DrawRect(progress_rect, false);
//
//			Drawing.UpdateWindow();
		}
	} while (Assets.IsLoading());

	Strings::PrintDivider();

	// 3d stuff
	SDL_GPUTexture *character_gpu_texture = sTest3DTexture;
	SDL_GPUTexture *planet_gpu_texture = sPlanetTexture;

	Gamepads.AddMissingGamepads();
	GameReference.Initialize();

	EventOnce<>& pre_render_event = Application.GetPreRenderEvent();

	// Do this after assets have been loaded because it uses them
	Menus.Initialize();
	Menus.main_menu->SwitchToThisMenu();

	DrawCall draw_vignette(Quad::NUM_VERTICES, Quad::NUM_INDICES, sVignette);
	Quad vignette_quad(
		draw_vignette,
		Dim2Rect(
			Rect4f(0, 0, 0, 0),
			Rect4f(0, 0, 1, 1)
		),
		SDL_Color(0, 0, 0, 200)
	);
	pre_render_event.Subscribe([&draw_vignette]()
							   {
//								   draw_vignette.SetTranslation(Application.GetHalfResolution());
								   draw_vignette.UpdateGPU();
							   });

	Menus.GetMainMenu()->SwitchToThisMenu();

	Vec3f cubes_at = Vec3f(0.0f, 100.0f, 100.0f);
	std::vector<GPUMesh *> gpu_shapes;
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
			{
				Mesh new_cube = MeshPresets::CreateCube(10.0f);
				new_cube.position = Vec3f(
					-15.0f + (float)x * 15.0f,
					-15.0f + (float)y * 15.0f,
					-15.0f + (float)z * 15.f
				) + cubes_at;
				new_cube.gpu_texture = character_gpu_texture;
				gpu_shapes.push_back(Drawing.LoadMeshToGPU(new_cube));
			}

	Mesh sphere = MeshPresets::CreateSphere(30.0f, 30, 50);
	sphere.position = Vec3f(70.0f, 0.0f, 0.0f) + cubes_at;
	sphere.gpu_texture = planet_gpu_texture;
	gpu_shapes.push_back(Drawing.LoadMeshToGPU(sphere));

//	GPUMesh *gpu_sphere = Drawing.LoadMeshToGPU(sphere);

	GPUMesh *ground = Drawing.LoadMeshToGPU(MeshPresets::CreateGround(1000.0f, 100));
	ground->gpu_texture = character_gpu_texture;
	gpu_shapes.push_back(ground);

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

			}

			Vec3f camera_position = GameReference.GetCamera3D().GetPosition();
			ground->position = Vec3f(camera_position.x, 0, camera_position.z);

			if (Drawing.CreateCommandBuffer())
			{
				pre_render_event.Invoke();

				// 3d
				if (GameReference.World() && Drawing.BeginPass())
				{
					Drawing.SetPipeline3D();
					Drawing.ShaderTick();
					Drawing.SetViewportAndScissor();

					for (GPUMesh *mesh : gpu_shapes)
					{
						if (!mesh->vertex_buffer || !mesh->indice_buffer)
						{
							SDL_Log("ERROR: Mesh has invalid buffers");
							continue;
						}

						Mat4x4 model_matrix = Mat4x4::Translation(mesh->position); // todo: optimize
						Drawing.ShaderParams(model_matrix, mesh->gpu_texture != nullptr);
						Drawing.DrawTriangles(mesh->vertex_buffer, mesh->indice_buffer, mesh->indice_count, mesh->gpu_texture);
					}

					Drawing.EndPass();
				}

				// 2d
				Menus.PreRender();
				if (Drawing.BeginPass2D())
				{
					Drawing.SetPipeline2D();
					Drawing.ShaderTick2D();

					if (GameReference.World())
						GameReference.World()->Draw();
					GameReference.GetInterface()->DrawBackground();

					draw_vignette.Draw();

					Menus.Render();
					GameReference.GetInterface()->DrawForeground();

//					Drawing.SetPipeline2DT();
//					Menus.RenderTransparent();

					Drawing.EndPass();
				}

				Drawing.EndCommandBuffer();
			}
		}

		total_frame_duration = clock->GetTimeElapsedNanoNow();
	}

	exit_application();
	return 0;
}
