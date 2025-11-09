//
// Created by Matq on 09/11/2025.
//

#include "CommonMesh.h"
#include <client/core/drawing/Drawing.h>

namespace CommonMesh
{
LoadTexture character_mesh_texture("entity.character.body_outline", AssetsClass::TexturePurpose::OBJECT_3D);
Mesh character_mesh;

void Initialize()
{
	character_mesh = MeshPresets::CreateTexturedCube(1.0f);
	character_mesh.SetTexture(character_mesh_texture);
	Drawing.QueueUpdate(&character_mesh);
}
}