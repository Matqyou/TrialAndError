//
// Created by Matq on 04/11/2025.
//

#pragma once

#include "client/core/texture/visual/HitboxFile.h"
#include "client/core/texture/Surface.h"

class VisualSurface : public Surface
{
private:
	Rect4f physical_hitbox;
	Rect4f translation_ratios;

public:
	VisualSurface(SDL_Surface *surface, const Rect4f& hitbox);
	~VisualSurface() override;

	// Getting
	[[nodiscard]] const Rect4f& GetHitbox() const { return physical_hitbox; }
	[[nodiscard]] const Rect4f& GetTranslationRatios() const { return translation_ratios; }
	[[nodiscard]] Rect4f GetResultingPlacement(const Rect4f& physical_placement) const;

};
