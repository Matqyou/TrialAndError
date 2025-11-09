// src/game/interface/MainMenu.h

#pragma once

#include <client/game/ui/menus/base/FullscreenMenu.h>
#include <client/core/drawing/mesh/Quad.h>
#include <shared/math/Vec2.h>
#include <SDL3/SDL.h>
#include <chrono>

class MainMenu : public FullscreenMenu
{
private:
	std::chrono::steady_clock::time_point opened_at;
	bool intro;

	Mesh2D mesh;
	Quad background;
	Mesh2D mesh_circles;
	Quad intro_circle;

public:
	MainMenu();
	~MainMenu() override;

	// Manipulating
	void SwitchToThisMenu() override;

	// Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary) override;
	void Tick(double elapsed_seconds) override;
	void PreRender() override;
	void Render() override;

};