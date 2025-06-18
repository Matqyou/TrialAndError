//
// Created by Matq on 16/04/2023.
//

#pragma once

#include "shared/utility/Colors.h"
#include "client/core/Drawing.h"

class TileMap;
struct Tile
{
	SDL_Color m_Color;
};

class TileMap
{
private:
	Drawing *m_Render;
	unsigned int m_Width, m_Height, m_Area;
	unsigned int m_Tilesize;
	Tile **m_Map;

public:
	TileMap(Drawing *render, int tilesize, int width, int height);
	~TileMap();

	// Getting
	[[nodiscard]] unsigned int Width() const { return m_Width; }
	[[nodiscard]] unsigned int Height() const { return m_Height; }
	[[nodiscard]] unsigned int Tilesize() const { return m_Tilesize; }
	[[nodiscard]] unsigned int TotalWidth() const { return m_Width * m_Tilesize; }
	[[nodiscard]] unsigned int TotalHeight() const { return m_Height * m_Tilesize; }

	// Manipulating
	void ClearTilemap();
	void SaveTilemap(const char *filepath);
	void LoadTilemap(const char *filepath);

	// Ticking
	void Draw();
};
