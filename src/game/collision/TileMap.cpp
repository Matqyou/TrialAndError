//
// Created by Matq on 16/04/2023.
//

#include "TileMap.h"

TileMap::TileMap(Drawing* render, int tilesize, int width, int height)
 : m_Tilesize(tilesize),
   m_Width(width),
   m_Height(height),
   m_Area(width * height) {
    m_Render = render;
    m_Map = new Tile*[m_Area];
    memset(m_Map, 0, sizeof(Tile*)*m_Area);

    for (int i = 0; i < m_Area; i++) {
        if (rand()%10 > 0)
            continue;

        m_Map[i] = new Tile();
        m_Map[i]->m_Color = {Uint8(rand()%255), Uint8(rand()%255), Uint8(rand()%255), Uint8(rand()%255)};
    }

}

TileMap::~TileMap() {
    for (int i = 0; i < m_Area; i++)
        delete m_Map[i];

    delete m_Map;
}

void TileMap::Draw() {
    for (int y = 0; y < m_Height; y++) {
        for(int x = 0; x < m_Width; x++) {
            Tile* DrawTile = m_Map[y * m_Width + x];
            if (!DrawTile)
                continue;

            SDL_Rect DrawRect = { x * m_Tilesize, y * m_Tilesize, m_Tilesize, m_Tilesize };
            m_Render->SetColor(DrawTile->m_Color);
            m_Render->FillRectWorld(DrawRect);
        }
    }
}