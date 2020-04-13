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
        SkScalar scaleFactor = 1;
        SkScalar scaleSpeed = 0.1f;
    public:
        void mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window);
        void mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window);
        void mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window);
        void mouseWheel(SDL_MouseWheelEvent& event, SDLSkiaWindow& window);
    };