//
// Created by Matq on 12/01/2025.
//

#pragma once

#include "client/game/ui/elements/Element.h"

class FullscreenMenu : public Element {
public:
    FullscreenMenu();

    // Ticking
    void FullscreenEvent(const SDL_Event& sdl_event, EventContext& event_summary);
    void HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary) override;

    // Manipulation
    void RefreshMenu();
	virtual void SwitchToThisMenu();

};

