//
// Created by Matq on 09/11/2025.
//

#pragma once

#include <client/core/drawing/mesh/Mesh2D.h>
#include <shared/screen/Dim2Rect.h>

class Quad
{
public:
	static const Uint32 NUM_VERTICES;
	static const Uint32 NUM_INDICES;

private:
	GPUVertex2D *vertices;
	uint32_t *indices;

public:
	Quad();
	Quad(GPUVertex2D *vertices_address, uint32_t *indices_address, const Dim2Rect& start_rect, const SDL_Color& start_color = { 255, 255, 255, 255 });
	Quad(Mesh2D& mesh_2d, const Dim2Rect& start_rect, const SDL_Color& start_color = { 255, 255, 255, 255 });

	GPUVertex2D& TopLeft() { return vertices[0]; }
	GPUVertex2D& TopRight() { return vertices[1]; }
	GPUVertex2D& BottomLeft() { return vertices[2]; }
	GPUVertex2D& BottomRight() { return vertices[3]; }

	void Allocate(GPUVertex2D *vertices_address, uint32_t *indices_address);
	void Allocate(Mesh2D& mesh_2d);
	void UpdateRect(const Dim2Rect& new_rect);
	void UpdateColor(const SDL_Color& new_color);
};