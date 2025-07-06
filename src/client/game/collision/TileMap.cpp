//
// Created by Matq on 16/04/2023.
//

#include "client/game/GameReference.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "TileMap.h"

TileMap::TileMap(int tilesize, int width, int height)
	: m_Tilesize(tilesize),
	  m_Width(width),
	  m_Height(height),
	  m_Area(width * height)
{
	m_Map = new Tile *[m_Area];
	memset(m_Map, 0, 8 * m_Area);

	//for (int y = 0; y < m_Height; y++) {
	//    for (int x = 0; x < m_Width; x++) {
	//        if (y == 0 || y == m_Height - 1 ||
	//            x == 0 || x == m_Width - 1) {
//
	//            Tile *NewTile = new Tile();
	//            NewTile->m_Color = { 0, 0, 0 };
	//            m_Map[y * m_Width + x] = NewTile;
	//        }
	//    }
	//}
}

TileMap::~TileMap()
{
	ClearTilemap();
	delete[] m_Map;
}

void TileMap::ClearTilemap()
{
	for (int i = 0; i < m_Area; i++)
	{
		delete m_Map[i];
		m_Map[i] = nullptr;
	}
}

void TileMap::SaveTilemap(const char *filepath)
{
	std::printf("====== Save %s ======\n", filepath);
	std::printf("Width: %i | Height: %i\n", m_Width, m_Height);
	std::printf("Area: %i | Real Area: %i\n", m_Area, m_Width * m_Height);

	int Tiles = 0;
	for (int i = 0; i < m_Area; i++)
		if (m_Map[i])
			Tiles++;

	std::printf("Solid: %i\n", Tiles);

	std::ofstream File(filepath, std::ios::binary);
	if (!File.is_open())
	{
		std::printf("Error opening the save file\n");
		return;
	}

	File.write((char *)(&m_Width), sizeof(m_Width));
	File.write((char *)(&m_Height), sizeof(m_Height));

	const unsigned char Zero = 0;
	const unsigned char One = 5;
	Tiles = 0;
	for (int i = 0; i < m_Area; i++)
	{
		Tile *pTile = m_Map[i];
		if (!pTile)
		{
			File.write((char *)(&Zero), sizeof(Zero));
			continue;
		}

		SDL_Color *Color = &pTile->m_Color;

		File.write((char *)(&One), sizeof(One));
		File.write((char *)(&Color->r), sizeof(Color->r));
		File.write((char *)(&Color->g), sizeof(Color->g));
		File.write((char *)(&Color->b), sizeof(Color->b));

		Tiles++;
	}

	File.close();
}

void TileMap::LoadTilemap(const char *filepath)
{
	if (!std::filesystem::exists(filepath))
	{
		std::printf("No such file found: %s", filepath);
		return;
	}

	std::printf("====== Load %s ======\n", filepath);

	int tWidth, tHeight, tArea;

	std::ifstream File(filepath, std::ios::binary);
	if (!File.is_open())
	{
		std::printf("Error opening the save file\n");
		return;
	}

	File.read((char *)(&tWidth), sizeof(tWidth));
	File.read((char *)(&tHeight), sizeof(tHeight));
	tArea = tWidth * tHeight;
	std::printf("Width: %i | Height: %i\n", tWidth, tHeight);
	std::printf("Area: %i \n", tArea);

	Tile **tMap = new Tile *[tArea];
	unsigned char tExists;
	for (int i = 0; i < tArea; i++)
	{
		File.read((char *)(&tExists), sizeof(tExists));
		if (tExists)
		{
			Tile *pTile = new Tile();
			SDL_Color *Color = &pTile->m_Color;

			File.read((char *)(&Color->r), sizeof(Color->r));
			File.read((char *)(&Color->g), sizeof(Color->g));
			File.read((char *)(&Color->b), sizeof(Color->b));

			tMap[i] = pTile;
		}
		else
		{ tMap[i] = nullptr; }
	}

	std::streampos Pos = File.tellg();
	File.seekg(0, std::ios::end);
	std::streampos EndPos = File.tellg();
	long long Remaining = EndPos - Pos;
	std::printf("Unread bytes: %lld\n", Remaining);

	ClearTilemap();
	delete m_Map;
	m_Map = tMap;
	m_Width = tWidth;
	m_Height = tHeight;
	m_Area = tWidth * tHeight;

	File.close();
}

void TileMap::Draw()
{
	auto drawing = Application.GetDrawing();
	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			Tile *DrawTile = m_Map[y * m_Width + x];
			if (!DrawTile)
				continue;

			SDL_FRect DrawRect = { float(x * m_Tilesize), float(y * m_Tilesize),
								   float(m_Tilesize), float(m_Tilesize) };
			drawing->SetColor(DrawTile->m_Color);
			drawing->DrawRect(DrawRect, true, GameReference.GetCamera());
		}
	}
}