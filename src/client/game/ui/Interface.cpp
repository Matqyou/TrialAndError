//
// Created by Matq on 13/12/2024.
//

#include "Interface.h"

InterfaceElement::InterfaceElement(Interface *parent, bool foreground, bool active)
{
	m_Interface = parent;
	m_Foreground = foreground;
	m_Active = active;

	parent->AddElement(this);
}

Interface::Interface()
{

}

void Interface::AddElement(InterfaceElement *element)
{
	if (element->IsForeground()) m_ForegroundElements.push_back(element);
	else m_BackgroundElements.push_back(element);
}

void Interface::DrawBackground()
{
	for (auto element : m_ForegroundElements)
	{
		if (!element->IsActive())
			continue;

		element->Draw();
	}
}

void Interface::DrawForeground()
{
	for (auto element : m_ForegroundElements)
	{
		if (!element->IsActive())
			continue;

		element->Draw();
	}
}