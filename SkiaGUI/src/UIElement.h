#pragma once
#include "pch.h"
#include "SDLSkiaWindow.h"

class UIElement;
typedef std::function<void(UIElement& e, SDL_WindowEvent& event, SDLSkiaWindow& window)> PResize;

class UIElement
    {
    public:
        UIElement* parent;
        SkRect rect;
        SkColor backgroundColor = SK_ColorTRANSPARENT;
        std::vector<UIElement*> children;
        virtual void drawAll(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window);
        UIElement& operator+=(UIElement* child);
        UIElement& operator+=(UIElement& child) { return *this += &child; }
        void trickleResizeEvent(SDL_WindowEvent& event, SDLSkiaWindow& window);
        PResize resize = nullptr;
        bool hitTest(int x, int y);
        SkRect absoluteRect();
    protected:
        void drawBackground(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window);
        void drawChildren(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window);
        virtual void drawMe(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window) {}
    };