#pragma once
#include "pch.h"
#include "SDLSkiaWindow.h"

typedef std::function<void(UIArea& e, SDL_WindowEvent& event)> PResize;
typedef std::function<void(UIArea& e, SDL_MouseMotionEvent& event)> PMouseMove;
typedef std::function<void(UIArea& e, SDL_MouseButtonEvent& event)> PMouseClick;

class View;
class UIArea
    {
    friend class SDLSkiaWindow;
    protected:
        SDLSkiaWindow* window = nullptr;
        View* view = nullptr;
    public:
        UIArea* parent = nullptr;
        SkRect rect;
        void drawAll();
        virtual void trickleResizeEvent(SDL_WindowEvent& event);
        virtual bool trickleMouseMoveEvent(SDL_MouseMotionEvent& event);
        virtual bool trickleMouseUpEvent(SDL_MouseButtonEvent& event);
        virtual bool trickleMouseDownEvent(SDL_MouseButtonEvent& event);
        virtual void trickleIdle() { onIdle(); }
        virtual void trickleKeyDown(SDL_KeyboardEvent& event) { keyDown(event); }
        virtual void trickleTextEvent(SDL_TextInputEvent& event) { textInput(event); }
        SkRect absoluteRect();
        virtual bool hitTest(SkScalar x, SkScalar y);
        SkMatrix totalTransform;
        void mapPixelsToPoints(SkPoint* points, int count);
        void mapPointsToPixels(SkPoint* dst, SkPoint* src, int count);
        SDLSkiaWindow* getWindow();
        View* getView();
        SkCanvas& Canvas() { return window->Canvas(); }
    protected:
        virtual void _drawMe() {}
        virtual void _resizeContent(SDL_WindowEvent& event) {}
        virtual void _mouseMove(SDL_MouseMotionEvent& event) {}
        virtual void _mouseUp(SDL_MouseButtonEvent& event) {}
        virtual void _mouseDown(SDL_MouseButtonEvent& event) {}
        virtual void onIdle() {}
        virtual void keyDown(SDL_KeyboardEvent& event) {}
        virtual void textInput(SDL_TextInputEvent& event) {}
    public:
        //event callbacks
        PResize resize = nullptr;
        PMouseMove mouseMove = nullptr;
        PMouseClick mouseUp = nullptr;
        PMouseClick mouseDown = nullptr;
        bool hasFocus() { return window->getClient().hasFocus(this); }
        void takeFocus(UIArea* el) { window->getClient().takeFocus(el); }
    };

