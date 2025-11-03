//
// Created by Matq on 16/12/2024.
//

#include "Element.h"
#include "client/core/Application.h"
#include <codecvt>
#include <ranges>

std::vector<Element *> Element::sDestroyElements = { };

void Element::DestroyElements()
{
	if (sDestroyElements.empty())
		return;

	for (auto destroy_element : sDestroyElements)
	{
		if (destroy_element->parent != nullptr)
		{
			auto parent_ = destroy_element->parent;
			auto it = std::find(parent_->children.begin(), parent_->children.end(), destroy_element);
			if (it != parent_->children.end())
				parent_->children.erase(it);
		}

		delete destroy_element;
	}

	sDestroyElements.clear();
}

Element::Element()
	: name("Element"),
	  texture_instance(nullptr)
{
	this->parent = nullptr;
	this->relative = Vec2i(0, 0);
	this->pos = Vec2i(0, 0);
	this->size = Vec2i(0, 0);;
	this->draw = DONT_DRAW;

	// Calculated
	this->edge = pos + size;

	// Default
	this->enabled = true;
	this->composition_pos = Vec2i(0, 0);
	this->flex_involved_horizontal = true;
	this->flex_involved_vertical = true;
	this->occupy_width = false;
	this->occupy_height = false;
	this->occupy_fully_width = false;
	this->occupy_fully_height = false;
	this->adaptive_width = false;
	this->adaptive_height = false;
	this->align_horizontal = Align::DONT;
	this->align_vertical = Align::DONT;
	this->flex = Flex::DONT;
	this->flex_gap = 0;
	this->has_focus = false;
	this->color = { 200, 150, 150, 255 };
	this->focus_color = { 200, 255, 200, 255 };

	this->flagged_to_destroy = false;
}

Element::~Element()
{
	for (auto child : children)
		delete child;
}

bool Element::PointCollides(int x, int y) const
{
	return !(x < this->pos.x || x > this->edge.x ||
		y < this->pos.y || y > this->edge.y);
}

Element *Element::SetChildren(const std::vector<Element *>& new_children)
{
	this->children.clear();
	for (auto new_child : new_children)
	{
		if (new_child == nullptr)
			continue;

		new_child->SetParent(this);
		this->children.push_back(new_child);
	}

	return this;
}

Element *Element::AddChildren(const std::vector<Element *>& new_children)
{
	for (auto child : new_children)
	{
		if (child == nullptr)
			continue;

		child->SetParent(this);
		this->children.push_back(child);
	}

	return this;
}

void Element::UnfocusChildren()
{
	for (auto child : children)
	{
		child->has_focus = false;
		child->UnfocusChildren();
	}
}

void Element::SetFocus(Element *focus_element)
{
	for (auto child : children)
		child->has_focus = false;

	if (focus_element != nullptr)
		focus_element->has_focus = true;
}

void Element::UpdateElement(const Vec2i& new_pos, const Vec2i& new_size)
{
	pos = new_pos;
	size = new_size;
	edge = pos + size;

	Refresh();
}

void Element::AlignHorizontal_()
{
	// Parent check
	if (align_horizontal != Align::DONT)
	{
		switch (align_horizontal)
		{
			case Align::BEHIND_LEFT:
			{
				composition_pos.x = -size.x;
				break;
			}
			case Align::LEFT:
			{
				composition_pos.x = 0;
				break;
			}
			case Align::RIGHT:
			{
				composition_pos.x = parent->size.x - size.x;
				break;
			}
			case Align::BEHIND_RIGHT:
			{
				composition_pos.x = parent->size.x;
				break;
			}
			case Align::CENTER:
			{
				composition_pos.x = (parent->size.x - size.x) / 2;
				break;
			}
			case Align::DONT: break;
		}
	}
}

void Element::AlignVertical_()
{
	// Parent check
	if (align_vertical != Align::DONT)
	{
		switch (align_vertical)
		{
			case Align::ABOVE_TOP:
			{
				composition_pos.y = -size.y;
				break;
			}
			case Align::TOP:
			{
				composition_pos.y = 0;
				break;
			}
			case Align::BOTTOM:
			{
				composition_pos.y = parent->size.y - size.y;
				break;
			}
			case Align::UNDER_BOTTOM:
			{
				composition_pos.y = parent->size.y;
				break;
			}
			case Align::CENTER:
			{
				composition_pos.y = (parent->size.y - size.y) / 2;
				break;
			}
			case Align::DONT: break;
		}
	}
}

int Element::GetFlexSlice()
{
	int flex_negative_space = 0;
	int flex_num_children = 0;
	int num_children = 0;
	for (auto child : children)
	{
		if (!child->enabled)
			continue;

		if (flex == Flex::WIDTH && child->flex_involved_horizontal)
		{
			// The number of dynamic width children
			if (child->occupy_width) flex_num_children++;
				// Flex volume in static children's width, count adaptive width, take the largest for height
			else flex_negative_space += child->size.x;
			num_children++;
		}
		else if (flex == Flex::HEIGHT && child->flex_involved_vertical)
		{
			if (child->occupy_height) flex_num_children++;
			else flex_negative_space += child->size.y;
			num_children++;
		}
	}

	int flex_space = (flex == Flex::WIDTH ? size.x : size.y) - flex_negative_space;
	return flex_num_children > 0 ? (flex_space - (num_children - 1) * flex_gap) / flex_num_children : 0;
}

void Element::FlexChildHorizontal_(Element *child, int flex_slice, int& current_flex) const
{
	child->composition_pos.x = 0;

	if (this->flex == Flex::WIDTH && child->flex_involved_horizontal)
	{
		if (child->occupy_width)
		{
			child->composition_pos.x = current_flex;
			child->size.x = flex_slice;
			current_flex += flex_slice;
		}
		else
		{
			child->composition_pos.x = current_flex;
			current_flex += child->size.x;
		}
		current_flex += this->flex_gap;
	}
}

void Element::FlexChildVertical_(Element *child, int flex_slice, int& current_flex) const
{
	child->composition_pos.y = 0;

	if (this->flex == Flex::HEIGHT && child->flex_involved_vertical)
	{
		if (child->occupy_height)
		{
			child->composition_pos.y = current_flex;
			child->size.y = flex_slice;
			current_flex += flex_slice;
		}
		else
		{
			child->composition_pos.y = current_flex;
			current_flex += child->size.y;
		}
		current_flex += this->flex_gap;
	}

}

void Element::Refresh(int child_generation)
{
	{ // 1st gen - adaptive resizing
		int adaptive_w = 0;
		int adaptive_h = 0;
		int num_children = 0;
		for (auto child : children)
		{
			if (!child->enabled)
				continue;

			if (child->occupy_fully_width)
			{
				child->size.x = size.x;
			}
			if (child->occupy_fully_height)
			{
				child->size.y = size.y;
			}
			child->Refresh(child_generation + 1);

			if (flex == Flex::WIDTH)
			{
				if (!child->occupy_width && child->flex_involved_horizontal)
				{
					num_children++;
					adaptive_w += child->size.x;
					if (child->size.y > adaptive_h)
						adaptive_h = child->size.y;
				}
			}
			else if (flex == Flex::HEIGHT && child->flex_involved_vertical)
			{
				num_children++;
				if (!child->occupy_height)
				{
					adaptive_h += child->size.y;
					if (child->size.x > adaptive_w)
						adaptive_w = child->size.x;
				}
			}
			else
			{
				if (child->size.x > adaptive_w && child->flex_involved_horizontal)
				{ adaptive_w = child->size.x; }
				if (child->size.y > adaptive_h && child->flex_involved_vertical)
				{ adaptive_h = child->size.y; }
			}
		}

		if (adaptive_width)
		{
			adaptive_w += flex == Flex::WIDTH ? (num_children - 1) * flex_gap : 0;
			size.x = adaptive_w;
		}
		if (adaptive_height)
		{
			adaptive_h += flex == Flex::HEIGHT ? (num_children - 1) * flex_gap : 0;
			size.y = adaptive_h;
		}
	}
	{ // 1st gen - flex and align
		if (this->parent == nullptr)
		{
			int flex_slice = this->GetFlexSlice();
			int current_flex = 0;
			for (auto child : this->children)
			{
				if (!child->enabled)
					continue;

				// Flex
				FlexChildHorizontal_(child, flex_slice, current_flex);
				FlexChildVertical_(child, flex_slice, current_flex);

				// Align children
				child->AlignHorizontal_();
				child->AlignVertical_();
			}
		}
	}

	// 2nd gen - flex and align
	for (auto parent : children)
	{
		if (!parent->enabled) continue;

		int flex_slice = parent->GetFlexSlice();
		int current_flex = 0;
		for (auto child : parent->children)
		{
			if (!child->enabled)
				continue;

			// Flex
			if (!(parent->occupy_width && child->occupy_width))
				parent->FlexChildHorizontal_(child, flex_slice, current_flex);
			if (!(parent->occupy_height && child->occupy_height))
				parent->FlexChildVertical_(child, flex_slice, current_flex);

			// Align
			if (!parent->occupy_width || this->parent == nullptr) child->AlignHorizontal_();
			if (!parent->occupy_height || this->parent == nullptr) child->AlignVertical_();

			// 3rd gen - align
			for (auto grandchild : child->children)
			{
				if (!grandchild->enabled)
					continue;

				// Align
				if (child->occupy_width) grandchild->AlignHorizontal_();
				if (child->occupy_height) grandchild->AlignVertical_();
			}
		}
	}

	// Last Step - Update coordinates, since all we did was relative up to this point
	if (child_generation == 0)
		UpdateComposition();
}

void Element::UpdateComposition()
{
	PreComposition();
	if (parent != nullptr)
	{
		pos = parent->pos + composition_pos + relative;
		edge = pos + size;
	}

	// Update positions for objects using gathered composition_pos and parents coordinates
	for (auto child : children)
		child->UpdateComposition();
	PostRefresh();
}

std::string WStringToUTF8(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}

void Element::DebugPrint(std::vector<bool> level, bool last_child)
{
	// Initialize the indentation string
	std::string indent;

	// Build the indentation for current level
	for (size_t i = 0; i < level.size(); ++i)
	{
		if (i < level.size() - 1)
		{
			indent += !level[i + 1] ? " │  " : "    "; // Draw vertical line or space
		}
		else
		{
			indent += last_child ? " └─>" : " ├─>"; // Draw branch or last branch
		}
	}

	// Add the current level's status to the stack
	level.push_back(last_child);

	// Print the current element
	std::string output_element = Strings::FStringColors("%s\"%s&r\"", indent.c_str(), name.c_str());

	if (relative.x != 0 || relative.y != 0)
	{
		const char *x_sign = relative.x > 0 ? "+" : "";
		const char *y_sign = relative.y > 0 ? "+" : "";
		output_element += Strings::FStringColors(" (&d%s%i&r,&d%s%i&r)", x_sign, relative.x, y_sign, relative.y);
	}
	const char *x_color = parent != nullptr && parent->flex == Flex::WIDTH ? "&5" : "&6";
	const char *y_color = parent != nullptr && parent->flex == Flex::HEIGHT ? "&5" : "&6";
	const char *x_suffix = align_horizontal == Align::DONT ? "x" :
						   align_horizontal == Align::LEFT ? "←" :
						   align_horizontal == Align::RIGHT ? "→" :
						   "⇆";
	const char *y_suffix = align_vertical == Align::DONT ? "y" :
						   align_vertical == Align::TOP ? "↑" :
						   align_vertical == Align::BOTTOM ? "↓" :
						   "⇅";
	output_element +=
		Strings::FStringColors(" (%s%i%s&r,%s%i%s&r)", x_color, pos.x, x_suffix, y_color, pos.y, y_suffix);
	if (flex == Flex::WIDTH) output_element += "←→";
	else if (flex == Flex::HEIGHT) output_element += " ↕ ";
	else output_element += " ";
	std::string width_color =
		flex != Flex::DONT && adaptive_width ? "&d" : occupy_width ? "&5" : occupy_fully_width ? "&c" : "&6";
	std::string height_color =
		flex != Flex::DONT && adaptive_height ? "&d" : occupy_height ? "&5" : occupy_fully_height ? "&c" : "&6";
	output_element +=
		Strings::FStringColors("(%s%iw&r,%s%ih&r)", width_color.c_str(), size.x, height_color.c_str(), size.y);

	const char *output_draw = draw == DRAW_RECT ? "&aRECT □" :
							  draw == DRAW_TEXTURE ? "&aVISUAL ■" :
							  "&c□";
	output_element += Strings::FStringColors(" %s", output_draw);

	output_element += Strings::FStringColors("%s", has_focus ? " &bFOCUSED" : "");

	std::cout << output_element << "\n";

	// Recurse into children
	int index = 0;
	int num_children = (int)children.size();
	for (auto child : children)
	{
		child->DebugPrint(level, ++index == num_children);
	}
}

void Element::BaseRender()
{
	auto drawing = Application.GetDrawing();
	switch (draw)
	{
		case DRAW_RECT:
		{
			auto& fill_color = has_focus ? focus_color : color;
			drawing->SetColor(fill_color);
			drawing->DrawRect(GetRect(), true);
			break;
		}
		case DRAW_TEXTURE:
		{
			auto texture = texture_instance.GetTexture() != nullptr ? texture_instance.GetTexture() : Assets.GetInvalidTexture();
			auto sdl_texture = texture->SDLTexture() != nullptr ? texture->SDLTexture() : Assets.GetInvalidTexture()->SDLTexture();
			drawing->RenderTexture(sdl_texture, nullptr, texture_instance.GetResultingFRect());
			break;
		}
		default: break;
	}
}

void Element::Tick(double elapsed_seconds)
{
	TickChildren(elapsed_seconds);
}

void Element::HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary)
{
	HandleEventChildren(sdl_event, event_summary);
}

void Element::Render()
{
	BaseRender();
	RenderChildren();
}

void Element::FlagToDestroy()
{
	if (!this->flagged_to_destroy)
	{
		this->flagged_to_destroy = true;

		sDestroyElements.push_back(this);
	}
}

void Element::RenderDebug()
{
	auto drawing = Application.GetDrawing();

	drawing->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
	if (draw == DRAW_TEXTURE)
	{
		drawing->SetColor(0, 255, 0, 255);
		drawing->DrawRect(texture_instance.GetResultingFRect(), false);
	}

	SDL_Color debug_color;
	if (occupy_width || occupy_height) debug_color = { 128, 0, 255 };
	else debug_color = { 255, 0, 0 };
	drawing->SetColor(debug_color.r, debug_color.g, debug_color.b, 10);
	drawing->DrawRect(GetRect(), true);

	drawing->SetColor(debug_color.r, debug_color.g, debug_color.b, 255);
	drawing->DrawRect(GetRect(), false);

	RenderDebugChildren();
}

void Element::PostEvent()
{
	PostEventChildren();
}

void Element::PreComposition()
{

}

void Element::PostRefresh()
{
	if (draw == DRAW_TEXTURE)
		UpdateTexturePlacement();
}

void Element::PostEventChildren() const
{
	for (auto child : children)
	{
		if (!child->enabled)
			continue;

		child->PostEvent();
	}
}

void Element::TickChildren(double elapsed_seconds) const
{
	for (auto child : children)
	{
		if (!child->enabled)
			continue;

		child->Tick(elapsed_seconds);
	}
}

void Element::HandleEventChildren(const SDL_Event& sdl_event, EventContext& event_summary)
{
	for (auto child : std::ranges::reverse_view(children))
	{
		if (!child->enabled)
			continue;

		child->HandleEvent(sdl_event, event_summary);
	}
}

void Element::RenderChildren() const
{
	for (auto child : children)
	{
		if (!child->enabled)
			continue;

		child->Render();
	}
}

void Element::RenderDebugChildren() const
{
	for (auto child : children)
	{
		if (!child->enabled)
			continue;

		child->RenderDebug();
	}
}

void Element::UpdateTexturePlacement()
{
	texture_instance.UpdateWithNewPlacement(GetRect());
}
