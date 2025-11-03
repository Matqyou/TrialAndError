//
// Created by Matq on 19/12/2024.
//

#pragma once

#include <iostream>
#include "shared/math/Vec2.h"
#include "client/core/drawing/Drawing.h"
#include "client/core/Clock.h"
#include "client/core/Assets.h"
#include "shared/utility/Randomizer.h"
#include "client/game/ui/EventContext.h"

class ApplicationClass
{
public:
	enum class Status
	{
		UNINITIALIZED,
		INITIALIZED,
	};

protected:
	SDL_Window *window;
	SDL_Renderer *renderer;

	NonBlockingClock clock;
	Drawing *drawing;
	Randomizer *randomizer;

	Vec2i resolution;
	Vec2f half_resolution;
	static void PrintVersions();

	Status status;

public:
	ApplicationClass();
	void Initialize(const char *title,
					const char *version,
					const char *identifier,
					const Vec2i& window_resolution,
					double framerate, double idle_framerate,
					const char *renderer_backend = nullptr);
	void Destroy();
	~ApplicationClass();

	// Getting
	[[nodiscard]] SDL_Window *GetWindow() { return window; }
	[[nodiscard]] SDL_Renderer *GetRenderer() { return renderer; }
//	[[nodiscard]] SDL_GLContext& GlContext() { return m_GLContext; }
	[[nodiscard]] NonBlockingClock *GetClock() { return &clock; }
	[[nodiscard]] Drawing *GetDrawing() { return drawing; }
	[[nodiscard]] Randomizer *GetRandomizer() const { return randomizer; }

	[[nodiscard]] Vec2i GetResolution() const { return resolution; }
	[[nodiscard]] Vec2f GetHalfResolution() const { return half_resolution; }
	[[nodiscard]] float GetAspectRatio() const { return static_cast<float>(resolution.x) / static_cast<float>(resolution.y); }
	[[nodiscard]] int GetWidth() const { return resolution.x; }
	[[nodiscard]] int GetHeight() const { return resolution.y; }
	[[nodiscard]] int GetWidth2() const { return resolution.x / 2; }
	[[nodiscard]] int GetHeight2() const { return resolution.y / 2; }

	[[nodiscard]] static Vec2f GetMousePosition();

	// Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_context);

};

extern ApplicationClass Application;