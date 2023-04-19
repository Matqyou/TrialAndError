//
// Created by Matq on 16/04/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_COLLISION_TILEMAP_H_
#define TRIALANDERROR_SRC_GAME_COLLISION_TILEMAP_H_

#include "../../technical stuff/Colors.h"
#include "../../technical stuff/Drawing.h"

class TileMap;
struct Tile {
    SDL_Color m_Color;
};

class TileMap {
private:
    Drawing* m_Render;
    unsigned int m_Width, m_Height, m_Area;
    unsigned int m_Tilesize;
    Tile** m_Map;

public:
    TileMap(Drawing* render, int tilesize, int width, int height);
    ~TileMap();

    unsigned int Width() const { return m_Width; }
    unsigned int Height() const { return m_Height; }
    unsigned int Tilesize() const { return m_Tilesize; }
    unsigned int TotalWidth() const { return m_Width * m_Tilesize; }
    unsigned int TotalHeight() const { return m_Height * m_Tilesize; }

    void ClearTilemap();
    void SaveTilemap(const char* filepath);
    void LoadTilemap(const char* filepath);

    void Draw();
};

#endif //TRIALANDERROR_SRC_GAME_COLLISION_TILEMAP_H_
