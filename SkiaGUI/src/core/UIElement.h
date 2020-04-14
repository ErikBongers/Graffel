#pragma once
#include "pch.h"
#include "SDLSkiaWindow.h"

class UIElement;
typedef std::function<void(UIElement& e, SDL_WindowEvent& event, SDLSkiaWindow& window)> PResize;
typedef std::function<void(UIElement& e, SDL_MouseMotionEvent& event, SDLSkiaWindow& window)> PMouseMove;
typedef std::function<void(UIElement& e, SDL_MouseButtonEvent& event, SDLSkiaWindow& window)> PMouseUp;

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
        bool trickleMouseMoveEvent(SDL_MouseMotionEvent& event, SDLSkiaWindow& window);
        bool trickleMouseUpEvent(SDL_MouseButtonEvent& event, SDLSkiaWindow& window);
        bool hitTest(int x, int y);
        SkRect absoluteRect();
        
        //event callbacks
        PResize resize = nullptr;
        PMouseMove mouseMove = nullptr;
        PMouseUp mouseUp = nullptr;

    protected:
        void drawBackground(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window);
        void drawChildren(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window);
        virtual void drawMe(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window) {}
    };