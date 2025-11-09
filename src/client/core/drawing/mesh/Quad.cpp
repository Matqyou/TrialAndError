//
// Created by Matq on 09/11/2025.
//

#include "Quad.h"

const Uint32 Quad::NUM_VERTICES = 4;
const Uint32 Quad::NUM_INDICES = 6;

Quad::Quad()
{
	vertices = nullptr;
	indices = nullptr;
}

Quad::Quad(GPUVertex2D *vertices_address, uint32_t *indices_address, const Dim2Rect& start_rect, const SDL_Color& start_color)
{
	Allocate(vertices_address, indices_address);
	UpdateRect(start_rect);
	UpdateColor(start_color);
}

Quad::Quad(Mesh2D& mesh_2d, const Dim2Rect& start_rect, const SDL_Color& start_color)
{
	Allocate(mesh_2d);
	UpdateRect(start_rect);
	UpdateColor(start_color);
}

void Quad::Allocate(GPUVertex2D *vertices_address, uint32_t *indices_address)
{
	vertices = vertices_address;
	indices = indices_address;

	//
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;
}

void Quad::Allocate(Mesh2D& mesh_2d)
{
	Allocate(
		mesh_2d.AllocateVertices(Quad::NUM_VERTICES),
		mesh_2d.AllocateIndices(Quad::NUM_INDICES)
	);
}

void Quad::UpdateRect(const Dim2Rect& new_rect)
{
	if (!vertices || !indices)
		throw std::runtime_error("Quad::UpdateRect() Missing vertices or indices");

	float x2 = new_rect.relative.x + new_rect.relative.w;
	float y2 = new_rect.relative.y + new_rect.relative.h;
	float sx2 = new_rect.scaled.x + new_rect.scaled.w;
	float sy2 = new_rect.scaled.y + new_rect.scaled.h;

	vertices[0].x = new_rect.relative.x;
	vertices[0].y = new_rect.relative.y;
	vertices[0].sx = new_rect.scaled.x;
	vertices[0].sy = new_rect.scaled.y;
	vertices[0].u = 0;
	vertices[0].v = 0;

	vertices[1].x = x2;
	vertices[1].y = new_rect.relative.y;
	vertices[1].sx = sx2;
	vertices[1].sy = new_rect.scaled.y;
	vertices[1].u = 1;
	vertices[1].v = 0;

	vertices[2].x = new_rect.relative.x;
	vertices[2].y = y2;
	vertices[2].sx = new_rect.scaled.x;
	vertices[2].sy = sy2;
	vertices[2].u = 0;
	vertices[2].v = 1;

	vertices[3].x = x2;
	vertices[3].y = y2;
	vertices[3].sx = sx2;
	vertices[3].sy = sy2;
	vertices[3].u = 1;
	vertices[3].v = 1;
}

void Quad::UpdateColor(const SDL_Color& new_color)
{
	for (int i = 0; i < Quad::NUM_VERTICES; i++)
		vertices[i].color = new_color;
}