//
// Created by Matq on 16/12/2024.
//

#pragma once

#include "client/core/texture/visual/VisualTexture.h"
#include "client/game/ui/elements/EventContext.h"
#include "client/core/drawing/Drawing.h"
#include "shared/utility/Rectangles.h"
#include "SDL3/SDL.h"

enum class Flex {
    DONT,
    WIDTH,
    HEIGHT,
};

enum class Orientation {
    VERTICAL,
    HORIZONTAL,
};

enum ElementDraw {
    DONT_DRAW,
    DRAW_RECT,
    DRAW_TEXTURE,
    DRAW_CUSTOM,
};

enum class SimpleAlign {
    DONT,
    LEFT,
    CENTER,
    RIGHT,
    TOP = LEFT,
    BOTTOM = RIGHT,
};

enum class Align {
    DONT,
    BEHIND_LEFT,
    LEFT,
    CENTER,
    RIGHT,
    BEHIND_RIGHT,
    ABOVE_TOP = BEHIND_LEFT,
    TOP = LEFT,
    BOTTOM = RIGHT,
    UNDER_BOTTOM = BEHIND_RIGHT,
};

struct Element {
protected:
    static std::vector<Element*> sDestroyElements;

public:
    Element* parent;
    std::vector<Element*> children;

    Vec2i composition_pos;
    void UpdateComposition();

    Vec2i relative, pos, size, edge;
    ElementDraw draw;
    VisualTextureInstance texture_instance;
    std::wstring name;

    bool enabled;
    bool flex_involved_horizontal, flex_involved_vertical;
    bool occupy_width, occupy_height;
    bool occupy_fully_width, occupy_fully_height;
    bool adaptive_width, adaptive_height;
    Align align_horizontal, align_vertical;
    Flex flex;
    int flex_gap;

    bool has_focus;
    SDL_Color color;
    SDL_Color focus_color;

    bool flagged_to_destroy;

    static void DestroyElements();

private:
    void AlignHorizontal_();
    void AlignVertical_();
    int GetFlexSlice();
    void FlexChildHorizontal_(Element* child, int flex_slice, int& current_flex) const;
    void FlexChildVertical_(Element* child, int flex_slice, int& current_flex) const;

public:
    Element();
    virtual ~Element();

    // Getting
    [[nodiscard]] VisualTextureInstance& GetVisualTexture() { return texture_instance; }
    [[nodiscard]] Vec2f GetSize() const { return Vec2f(size); }
    [[nodiscard]] SDL_FRect GetRect() const {
        return SDL_FRect((float)pos.x,
                         (float)pos.y,
                         (float)size.x,
                         (float)size.y);
    }

    // Generating
    [[nodiscard]] bool PointCollides(int x, int y) const;

    // Options
    Element* SetChildren(const std::vector<Element*>& children);
    Element* AddChildren(const std::vector<Element*>& children);
    Element* SetEnabled(bool enabled) {
        this->enabled = enabled;
        return this;
    }
    Element* SetRelative(const Vec2i& relative) {
        this->relative = relative;
        return this;
    }
    Element* SetSize(const Vec2i& size) {
        this->size = size;
        return this;
    }
    Element* SetDraw(ElementDraw new_draw) {
        this->draw = new_draw;
        return this;
    }
    Element* SetFlexInvolved(bool horizontal, bool vertical) {
        this->flex_involved_horizontal = horizontal;
        this->flex_involved_vertical = vertical;
        return this;
    }
    Element* SetOccupy(bool horizontal, bool vertical) {
        this->occupy_width = horizontal;
        this->occupy_height = vertical;
        return this;
    }
    Element* SetFullyOccupy(bool horizontal, bool vertical) {
        this->occupy_fully_width = horizontal;
        this->occupy_fully_height = vertical;
        return this;
    }
    Element* SetFlex(Flex flex) {
        this->flex = flex;
        return this;
    }
    Element* SetFlex(Flex flex, int flex_gap) {
        this->flex = flex;
        this->flex_gap = flex_gap;
        return this;
    }
    Element* SetAlign(Align horizontal, Align vertical) {
        this->align_horizontal = horizontal;
        this->align_vertical = vertical;
        return this;
    }
    Element* SetAdaptive(bool horizontal, bool vertical) {
        this->adaptive_width = horizontal;
        this->adaptive_height = vertical;
        return this;
    }
    Element* SetName(const char* name) {
        this->name = Strings::FStringW(L"%s", name);
        return this;
    }
    Element* SetName(const wchar_t* name) {
        this->name = name;
        return this;
    }
    Element* SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        this->color = { r, g, b, a };
        return this;
    }
    Element* SetFocusColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        this->focus_color = { r, g, b, a };
        return this;
    }
    Element* SetTexture(Texture* texture) {
        this->texture_instance.ChangeTexture(texture);
        return this;
    }
    Element* ResizeToTexture() {
        this->size = Vec2i(texture_instance.GetTexture()->GetSize());
        this->edge = pos + size;
        return this;
    }

    // Setting
    void SetParent(Element* parent) { this->parent = parent; };

    // Manipulating
    void UnfocusChildren();
    void SetFocus(Element* focus_element);
    void UpdateElement(const Vec2i& new_pos, const Vec2i& new_size);
    void Refresh(int child_generation = 0);
    void DebugPrint(std::vector<bool> level = { }, bool last_child = true);
    void FlagToDestroy();

    // Ticking
    void BaseRender();

    virtual void Tick(double elapsed_seconds);
    virtual void HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary);
    virtual void Render();
    virtual void RenderDebug();
    virtual void PostEvent();
    virtual void PreComposition();
    virtual void PostRefresh();

    void PostEventChildren() const;
    void TickChildren(double elapsed_seconds) const;
    void HandleEventChildren(const SDL_Event& sdl_event, EventContext& event_summary);
    void RenderChildren() const;
    void RenderDebugChildren() const;
    void UpdateTexturePlacement();

};