#pragma once
#include "pch.h"
#include "UIElement.h"

class InfiniteCanvas : public UIElement
    {
    private:
        void drawAll(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window);
        SkPoint startDrag;
        SkPoint startDragTranslate;
        bool isDragging = false;
        SkScalar xTranslate = 0, yTranslate = 0;
    public:
        void mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window);
        void mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window);
        void mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window);
    };