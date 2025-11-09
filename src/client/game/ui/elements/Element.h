//
// Created by Matq on 16/12/2024.
//

#pragma once

#include "client/core/texture/visual/VisualTexture.h"
#include "client/game/ui/EventContext.h"
#include "client/core/drawing/Drawing.h"
#include "shared/geometry/Rectangles.h"
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
    std::string name;

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
    Element* SetChildren(const std::vector<Element*>& new_children);
    Element* AddChildren(const std::vector<Element*>& new_children);
    Element* SetEnabled(bool new_enabled) {
        enabled = new_enabled;
        return this;
    }
    Element* SetRelative(const Vec2i& new_relative) {
        relative = new_relative;
        return this;
    }
    Element* SetSize(const Vec2i& new_size) {
        size = new_size;
        return this;
    }
    Element* SetDraw(ElementDraw new_draw) {
        draw = new_draw;
        return this;
    }
    Element* SetFlexInvolved(bool new_horizontal, bool new_vertical) {
        flex_involved_horizontal = new_horizontal;
        flex_involved_vertical = new_vertical;
        return this;
    }
    Element* SetOccupy(bool new_horizontal, bool new_vertical) {
        occupy_width = new_horizontal;
        occupy_height = new_vertical;
        return this;
    }
    Element* SetFullyOccupy(bool new_horizontal, bool new_vertical) {
        occupy_fully_width = new_horizontal;
        occupy_fully_height = new_vertical;
        return this;
    }
    Element* SetFlex(Flex new_flex) {
        flex = new_flex;
        return this;
    }
    Element* SetFlex(Flex new_flex, int new_flex_gap) {
        flex = new_flex;
        flex_gap = new_flex_gap;
        return this;
    }
    Element* SetAlign(Align new_horizontal, Align new_vertical) {
        align_horizontal = new_horizontal;
        align_vertical = new_vertical;
        return this;
    }
    Element* SetAdaptive(bool new_horizontal, bool new_vertical) {
        adaptive_width = new_horizontal;
        adaptive_height = new_vertical;
        return this;
    }
    Element* SetName(const char* new_name) {
        name = new_name;
        return this;
    }
    Element* SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        color = { r, g, b, a };
        return this;
    }
    Element* SetFocusColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        focus_color = { r, g, b, a };
        return this;
    }
    Element* SetTexture(Texture* texture) {
        texture_instance.ChangeTexture(texture);
        return this;
    }
    Element* ResizeToTexture() {
        size = Vec2i(texture_instance.GetTexture()->GetSize());
        edge = pos + size;
        return this;
    }

    // Setting
    void SetParent(Element* new_parent) { parent = new_parent; }

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
    virtual void PreRender();
    virtual void Render();
    virtual void RenderTransparent();
    virtual void RenderDebug();
    virtual void PostEvent();
    virtual void PreComposition();
    virtual void PostRefresh();

    void PostEventChildren() const;
    void TickChildren(double elapsed_seconds) const;
    void HandleEventChildren(const SDL_Event& sdl_event, EventContext& event_summary);
    void PreRenderChildren();
    void RenderChildren() const;
    void RenderTransparentChildren() const;
    void RenderDebugChildren() const;
    void UpdateTexturePlacement();

};