//
// Created by Matq on 16/04/2023.
//

#include "TileMap.h"
#include <fstream>

TileMap::TileMap(Drawing* render, int tilesize, int width, int height)
 : m_Tilesize(tilesize),
   m_Width(width),
   m_Height(height),
   m_Area(width * height) {
    m_Render = render;
    m_Map = new Tile*[m_Area];
    memset(m_Map, 0, 8 * m_Area);

    for (int i = 0; i < m_Area; i++) {
        if (rand() % 100 > 0)
            continue;

        Tile* NewTile = new Tile();
        NewTile->m_Color = {Uint8(rand()%255), Uint8(rand()%255), Uint8(rand()%255) };
        m_Map[i] = NewTile;
    }

}

TileMap::~TileMap() {
    ClearTilemap();
    delete m_Map;
}

void TileMap::ClearTilemap() {
    for (int i = 0; i < m_Area; i++) {
        delete m_Map[i];
        m_Map[i] = nullptr;
    }
}

void TileMap::SaveTilemap(const char* filepath) {
    std::ofstream File;
    File.open(filepath);
    File.write((char*)(&m_Width), sizeof(m_Width));
    File.write((char*)(&m_Height), sizeof(m_Height));
    for (int i = 0; i < m_Area; i++) {
        Tile* SaveTile = m_Map[i];
        bool Exists = bool(SaveTile);
        File.write((char*)(&Exists), sizeof(Exists));
        if (!Exists) continue;

        File.write((char*)(&SaveTile->m_Color), sizeof(Uint8) * 3);
    }
    File.close();
}
#include <sys/stat.h>
void TileMap::LoadTilemap(const char* filepath) {
    std::ifstream File(filepath);
    if (!File.is_open()) {
        std::cout << "Couldn't open tilemap for loading: " << filepath << std::endl;
        return;
    }

    ClearTilemap();

    File.seekg(0, std::ios::beg);

    File.read((char*)(&m_Width), sizeof(unsigned int));
    File.read((char*)(&m_Height), sizeof(unsigned int));
    m_Area = m_Width * m_Height;
    for (int i = 0; i < m_Area; i++) {
        bool Exists;
        File.read((char*)(&Exists), sizeof(Exists));
        if (!Exists) continue;

        Tile* NewTile = new Tile();
        m_Map[i] = NewTile;
        File.read((char*)(&NewTile->m_Color), sizeof(Uint8) * 3);
    }
    File.close();
}


void TileMap::Draw() {
    for (int y = 0; y < m_Height; y++) {
        for(int x = 0; x < m_Width; x++) {
            Tile* DrawTile = m_Map[y * m_Width + x];
            if (!DrawTile)
                continue;

            SDL_Rect DrawRect = { int(x * m_Tilesize), int(y * m_Tilesize),
                                  int(m_Tilesize), int(m_Tilesize) };
            m_Render->SetColor(DrawTile->m_Color);
            m_Render->FillRectWorld(DrawRect);
        }
    }
}