//
// Created by Matq on 13/12/2024.
//

#pragma once

#include <vector>
#include <iostream>

class Interface;
class InterfaceElement
{
protected:
	Interface *m_Interface;
	bool m_Foreground;
	bool m_Active;

public:
	explicit InterfaceElement(Interface *parent, bool foreground, bool active);

	// Getting
	[[nodiscard]] Interface *GetInterface() const { return m_Interface; }
	[[nodiscard]] bool IsForeground() const { return m_Foreground; }
	[[nodiscard]] bool IsActive() const { return m_Active; }

	// Manipulating
	void SetActive(bool active) { m_Active = active; }

	// Ticking
	virtual void Draw()
	{
		std::cout << "InterfaceElement::Draw() called" << std::endl;
	}

};

class GameData;
class Interface
{
private:
	std::vector<InterfaceElement *> m_ForegroundElements;
	std::vector<InterfaceElement *> m_BackgroundElements;

public:
	Interface();

	// Manipulating
	void AddElement(InterfaceElement *element);

	// Ticking
	void DrawBackground();
	void DrawForeground();

};
