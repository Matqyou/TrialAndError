//
// Created by Matq on 06/07/2025.
//

#pragma once

class Preferences
{
private:
	int player_class;

public:
	Preferences();
	~Preferences();

	// Sense
	[[nodiscard]] int GetPlayerClassType() const { return player_class; }

	// Manipulating
	void SetPlayerClass(int player_class_type) { player_class = player_class_type; }

};
