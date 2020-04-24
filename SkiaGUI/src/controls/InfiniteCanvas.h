#pragma once
#include "pch.h"
#include "core/UIElement.h"

class InfiniteCanvas : public UIElement
    {
    private:
        void drawAll();
        SkPoint dragStartMousePos;
        SkPoint dragStartDrageePos;
        bool isDragging = false;
        UIElement* dragee = nullptr;

        SkScalar xTranslate = 0, yTranslate = 0;
        SkScalar scaleFactor = 1;
        SkScalar scaleSpeed = 0.1f;

        SkMatrix baseTransform;
    
    public:
        void _mouseMove(SDL_MouseMotionEvent& event) override;
        void _mouseDown(SDL_MouseButtonEvent& event) override;
        void _mouseUp(SDL_MouseButtonEvent& event) override;
        void mouseWheel(SDL_MouseWheelEvent& event);
    };