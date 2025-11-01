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

Interface::~Interface()
{
//	for (InterfaceElement* element : foreground_elements)
//		delete element;
//	foreground_elements.clear();
//
//	for (InterfaceElement* element : background_elements)
//		delete element;
//	background_elements.clear();
}

void Interface::AddElement(InterfaceElement *element)
{
	if (element->IsForeground()) foreground_elements.push_back(element);
	else background_elements.push_back(element);
}

void Interface::DrawBackground()
{
	for (auto element : foreground_elements)
	{
		if (!element->IsActive())
			continue;

		element->Draw();
	}
}

void Interface::DrawForeground()
{
	for (auto element : foreground_elements)
	{
		if (!element->IsActive())
			continue;

		element->Draw();
	}
}