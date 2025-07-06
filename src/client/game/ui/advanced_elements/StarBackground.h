//
// Created by Matq on 22/06/2025.
//

#pragma once

#include "client/game/ui/elements/Element.h"

class StarBackground : public Element
{
private:
	double star_accumulator;
	std::vector<std::tuple<Vec2d, Vec2d, double>> m_Stars;

public:
	StarBackground();
	~StarBackground();

	// Ticking
	void Tick(double elapsed_seconds) override;
	void Render() override;

};
