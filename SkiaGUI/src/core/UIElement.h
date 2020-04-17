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
        virtual void drawAll(SDLSkiaWindow& window);
        UIElement& operator+=(UIElement* child);
        UIElement& operator+=(UIElement& child) { return *this += &child; }
        void trickleResizeEvent(SDL_WindowEvent& event, SDLSkiaWindow& window);
        bool trickleMouseMoveEvent(SDL_MouseMotionEvent& event, SDLSkiaWindow& window);
        bool trickleMouseUpEvent(SDL_MouseButtonEvent& event, SDLSkiaWindow& window);
        SkRect absoluteRect();
        virtual bool hitTest(SkScalar x, SkScalar y);
        SkMatrix totalTransform;
        
        //event callbacks
        PResize resize = nullptr;
        PMouseMove mouseMove = nullptr;
        PMouseUp mouseUp = nullptr;

    protected:
        void drawBackground(SDLSkiaWindow& window);
        void drawChildren(SDLSkiaWindow& window);
        virtual void drawMe(SDLSkiaWindow& window) {}
        virtual void _mouseMove(SDL_MouseMotionEvent& event, SDLSkiaWindow& window) {}
        virtual void _mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) {}
    };