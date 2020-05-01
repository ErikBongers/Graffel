#pragma once
#include "UIArea.h"

class UIElement : public UIArea
    {
    public:
        SkColor backgroundColor = SK_ColorTRANSPARENT;
        std::vector<UIArea*> children;
        UIElement& operator+=(UIArea* child);
        UIElement& operator+=(UIArea& child) { return *this += &child; }
        
        virtual void trickleResizeEvent() override;
        virtual bool trickleMouseMoveEvent(SDL_MouseMotionEvent& event) override;
        virtual bool trickleMouseUpEvent(SDL_MouseButtonEvent& event) override;
        virtual bool trickleMouseDownEvent(SDL_MouseButtonEvent& event) override;
        virtual void trickleIdle() override;
        virtual void trickleKeyDown(SDL_KeyboardEvent& event) override;
        virtual void trickleTextEvent(SDL_TextInputEvent& event) override;
        virtual void drawMe() {}

    protected:
        void drawBackground();
        void drawChildren();
        virtual void _drawMe() override;
    };