//
// Created by Matq on 17/01/2025.
//

#pragma once

#include "SDL3/SDL_mouse.h"

namespace Cursors {
extern SDL_Cursor* sCursorSystemText;
extern SDL_Cursor* sCursorSystemPointer;

void Initialize();
void Deinitialize();
}
