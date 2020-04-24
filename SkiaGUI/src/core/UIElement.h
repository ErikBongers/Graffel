#pragma once
#include "pch.h"
#include "SDLSkiaWindow.h"

class UIElement;
typedef std::function<void(UIElement& e, SDL_WindowEvent& event)> PResize;
typedef std::function<void(UIElement& e, SDL_MouseMotionEvent& event)> PMouseMove;
typedef std::function<void(UIElement& e, SDL_MouseButtonEvent& event)> PMouseClick;

class UIElement
    {
    friend class SDLSkiaWindow;
    private:
        SDLSkiaWindow* window = nullptr;
    public:
        UIElement* parent = nullptr;
        SkRect rect;
        SkColor backgroundColor = SK_ColorTRANSPARENT;
        std::vector<UIElement*> children;
        virtual void drawAll();
        UIElement& operator+=(UIElement* child);
        UIElement& operator+=(UIElement& child) { return *this += &child; }
        void trickleResizeEvent(SDL_WindowEvent& event);
        bool trickleMouseMoveEvent(SDL_MouseMotionEvent& event);
        bool trickleMouseUpEvent(SDL_MouseButtonEvent& event);
        bool trickleMouseDownEvent(SDL_MouseButtonEvent& event);
        void trickleIdle() { onIdle();  for (auto c : children) c->trickleIdle(); }
        void trickleKeyDown(SDL_KeyboardEvent& event) { keyDown(event);  for (auto c : children) c->trickleKeyDown(event); }
        void trickleTextEvent(SDL_TextInputEvent& event) { textInput(event);  for (auto c : children) c->trickleTextEvent(event); }
        SkRect absoluteRect();
        virtual bool hitTest(SkScalar x, SkScalar y);
        SkMatrix totalTransform;
        
        //event callbacks
        PResize resize = nullptr;
        PMouseMove mouseMove = nullptr;
        PMouseClick mouseUp = nullptr;
        PMouseClick mouseDown = nullptr;
        void mapPixelsToPoints(SkPoint* points, int count);
        void mapPointsToPixels(SkPoint* dst, SkPoint* src, int count);
        SDLSkiaWindow* getWindow();

    protected:
        void drawBackground();
        void drawChildren();
        virtual void drawMe() {}
        virtual void _resize(SDL_WindowEvent& event) {}
        virtual void _mouseMove(SDL_MouseMotionEvent& event) {}
        virtual void _mouseUp(SDL_MouseButtonEvent& event) {}
        virtual void _mouseDown(SDL_MouseButtonEvent& event) {}
        virtual void onIdle() {}
        virtual void keyDown(SDL_KeyboardEvent& event) {}
        virtual void textInput(SDL_TextInputEvent& event) {}
    };