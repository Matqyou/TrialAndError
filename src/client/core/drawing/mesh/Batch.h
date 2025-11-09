//
// Created by Matq on 09/11/2025.
//

#pragma once

#include <client/core/drawing/mesh/Mesh2D.h>
#include <client/core/drawing/mesh/Mesh.h>
#include <unordered_set>

class Batch
{
private:
	std::unordered_set<Mesh *> mesh_updates;
	std::unordered_set<Mesh2D *> mesh_updates_2d;

public:
	Batch() = default;
	~Batch();

	void AddUpdate(Mesh *update_mesh);
	void AddUpdate(Mesh2D *update_mesh_2d);
	void CancelUpdate(Mesh *cancel_update_mesh);
	void CancelUpdate(Mesh2D *cancel_update_mesh_2d);
	void UpdateGPU();

};