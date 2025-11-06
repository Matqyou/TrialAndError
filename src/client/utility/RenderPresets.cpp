//
// Created by Matq on 23/05/2025.
//

#include "RenderPresets.h"
#include "client/core/drawing/Drawing.h"
#include "client/core/Application.h"

Texture *RenderPresets::Composition(std::initializer_list<Texture *> textures, std::initializer_list<SDL_Color> colors)
{
//	if (textures.size() == 0 || *textures.begin() == nullptr)
//		return nullptr;
//
//	auto drawing = Application.GetDrawing();
//	auto original_render_target = SDL_GetRenderTarget(drawing->Renderer());
//
//	auto result = (*textures.begin())->CopyTexture(SDL_TEXTUREACCESS_TARGET);
//	drawing->SetRenderTarget(result);
//
//	std::vector<Texture*> texture_vec(textures);
//	std::vector<SDL_Color> color_vec(colors);
//
//	for (size_t i = 0; i < texture_vec.size(); i++)
//	{
//		auto texture = texture_vec[i];
//		auto color = (i < color_vec.size()) ? color_vec[i] : SDL_Color{255, 255, 255, 255};
//
//		if (!texture)
//			continue;
//
//		auto texture_copy = texture->CopyTexture(SDL_TEXTUREACCESS_TARGET)
//			->SetColorMod(color);
//		drawing->RenderTextureFullscreen(texture_copy->SDLTexture(), nullptr);
//		delete texture_copy;
//	}
//
//	drawing->SetRenderTargetSDL(original_render_target);
//	return result;
return nullptr;
}