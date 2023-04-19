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
    const int m_Width, m_Height, m_Area;
    const int m_Tilesize;
    Tile** m_Map;

public:
    TileMap(Drawing* render, int tilesize, int width, int height);
    ~TileMap();

    int Width() const { return m_Width; }
    int Height() const { return m_Height; }
    int Tilesize() const { return m_Tilesize; }
    int TotalWidth() const { return m_Width * m_Tilesize; }
    int TotalHeight() const { return m_Height * m_Tilesize; }

    void Draw();
};

#endif //TRIALANDERROR_SRC_GAME_COLLISION_TILEMAP_H_
