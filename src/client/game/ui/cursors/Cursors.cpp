//
// Created by Matq on 17/01/2025.
//

#include "Cursors.h"

#include "shared/string/Strings.h"

namespace Cursors {
SDL_Cursor* sCursorSystemText = nullptr;
SDL_Cursor* sCursorSystemPointer = nullptr;

void Initialize() {
    sCursorSystemText = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
    sCursorSystemPointer = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    dbg_msg("[Cursors] &5Loaded 2 cursors\n");
}

void Deinitialize() {
    SDL_DestroyCursor(sCursorSystemText);
    SDL_DestroyCursor(sCursorSystemPointer);
	dbg_msg("[Cursors] &5Unloaded 2 cursors\n");
}
}