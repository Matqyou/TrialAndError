//
// Created by Matq on 16/12/2024.
//

#include "Button.h"
#include "client/game/ui/cursors/Cursors.h"
#include "client/core/Application.h"

Button::Button()
	: Element(), pressed_texture_instance(nullptr)
{
	this->name = "Button";
	this->callback = { };
	this->clickable = true;
	this->pressed_down = false;

	this->SetDraw(DRAW_RECT);
}

Button::Button(Texture *texture, Texture *pressed_texture)
	: Element(), pressed_texture_instance(pressed_texture)
{
	this->name = "Button";
	this->callback = { };
	this->clickable = true;
	this->pressed_down = false;

	this->SetDraw(DRAW_TEXTURE);
	this->SetTexture(texture);
	this->SetPressedTexture(pressed_texture);
}

void Button::RunCallback() const
{
	if (callback != nullptr)
		callback();
}

void Button::PostRefresh()
{
	if (draw == DRAW_TEXTURE)
	{
		UpdateTexturePlacement();
		UpdatePressedVisualTexture();
	}
}

void Button::HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary)
{
	HandleEventChildren(sdl_event, event_summary);

	if (!event_summary.rapid_context.event_captured && clickable)
	{ // For the buttons it's pretty easy
		switch (sdl_event.type)
		{
			case SDL_EVENT_MOUSE_MOTION:
			{
				if (event_summary.cursor_changed == CursorChange::NO_CHANGE &&
					PointCollides((int)sdl_event.motion.x, (int)sdl_event.motion.y))
				{
					event_summary.cursor_changed = CursorChange::TO_POINTER;
					SDL_SetCursor(Cursors::sCursorSystemPointer);
					break;
				}

				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			{
				if (sdl_event.button.button == SDL_BUTTON_LEFT &&
					!event_summary.rapid_context.event_captured && PointCollides((int)sdl_event.button.x, (int)sdl_event.button.y))
				{
					event_summary.rapid_context.event_captured = true;
					RunCallback();
					pressed_down = true;
				}

				break;
			}
			case SDL_EVENT_MOUSE_BUTTON_UP:
			{
				if (sdl_event.button.button == SDL_BUTTON_LEFT)
					pressed_down = false;
				break;
			}
		}
	}
}

void Button::Render()
{
	auto drawing = Application.GetDrawing();
	switch (draw)
	{
		case DRAW_RECT:
		{
			auto& fill_color = has_focus ? focus_color : color;
			drawing->SetColor(fill_color);
			drawing->DrawRect(GetRect(), true);
			break;
		}
		case DRAW_TEXTURE:
		{
			// Lil cooked
			auto instance = pressed_down ? pressed_texture_instance : texture_instance;
			auto texture = instance.GetTexture() != nullptr ? instance.GetTexture() : Assets.GetInvalidTexture();
			auto sdl_texture = texture->SDLTexture() != nullptr ? texture->SDLTexture() : Assets.GetInvalidTexture()->SDLTexture();

			drawing->RenderTexture(sdl_texture, nullptr, instance.GetResultingFRect());
			break;
		}
		default: break;
	}

	RenderChildren();

}

void Button::UpdatePressedVisualTexture()
{
	pressed_texture_instance.UpdateWithNewPlacement(GetRect());
}
