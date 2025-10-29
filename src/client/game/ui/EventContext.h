//
// Created by Matq on 16/12/2024.
//

#pragma once

enum CursorChange
{
	NO_CHANGE,
	TO_ARROW,
	TO_HAND,
	TO_IBEAM,
	TO_POINTER,
	TO_CUSTOM,
};

struct EventContext
{
	struct RapidContext
	{
		bool event_captured;
	} rapid_context;

	bool had_mouse_motion;
	CursorChange cursor_changed;

	EventContext()
	{
		this->ResetContext();
		this->ResetRapidContext();
	}

	// Manipulation
	void ResetContext()
	{
		this->had_mouse_motion = false;
		this->cursor_changed = NO_CHANGE;
	}
	void ResetRapidContext()
	{
		this->rapid_context.event_captured = false;
	}

};
