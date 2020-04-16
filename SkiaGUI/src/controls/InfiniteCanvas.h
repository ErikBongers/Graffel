#pragma once
#include "pch.h"
#include "core/UIElement.h"

class InfiniteCanvas : public UIElement
    {
    private:
        void drawAll(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window);
        SkPoint dragStartMousePos;
        SkPoint dragStartDrageePos;
        bool isDragging = false;
        UIElement* dragee = nullptr;

        SkScalar xTranslate = 0, yTranslate = 0;
        SkScalar scaleFactor = 1;
        SkScalar scaleSpeed = 0.1f;
        inline SkMatrix createMatrix();
        inline void mapPixelsToPoints(SkPoint* dst, SkPoint* src, int count);
        inline void mapPointsToPixels(SkPoint* dst, SkPoint* src, int count);
    public:
        void _mouseMove(SDL_MouseMotionEvent& event, SDLSkiaWindow& window) override;
        void mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window);
        void _mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
        void mouseWheel(SDL_MouseWheelEvent& event, SDLSkiaWindow& window);
    };