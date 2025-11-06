// Copied Planet.h into cartesian
#pragma once

#include "client/game/entities/cartesian/base/Entity.h"

class Planet : public Entity
{
protected:
	float m_Radius;
	SDL_Color m_Color;
	std::string m_TextureKey;
	Texture *m_Texture{ nullptr };
	float m_Rotation = 0.0; // radians

public:
	void SetRotation(float r) { m_Rotation = r; }
	[[nodiscard]] float Rotation() const { return m_Rotation; }

public:
	Planet(const Vec3f& pos, float radius, SDL_Color color = { 120, 120, 255, 255 }, std::string texture_key = "");
	~Planet() override;

	[[nodiscard]] float Radius() const { return m_Radius; }

	void Tick(double elapsed_seconds) override;
	void Draw() override;
};