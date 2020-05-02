#pragma once
#include "pch.h"
#include "core/UIElement.h"

class InfiniteCanvas : public UIElement
    {
    protected:
        void _drawMe() override;
    private:
        SkPoint dragStartMousePos;
        SkPoint dragStartDrageePos;
        bool isDragging = false;
        UIArea* dragee = nullptr;

        SkScalar xTranslate = 0, yTranslate = 0;
        SkScalar scaleFactor = 1;
        SkScalar scaleSpeed = 0.1f;

        SkMatrix baseTransform;
    
    public:
        void _mouseMove(SDL_MouseMotionEvent& event) override;
        void _mouseDown(SDL_MouseButtonEvent& event) override;
        void _mouseUp(SDL_MouseButtonEvent& event) override;
        void _mouseWheel(SDL_MouseWheelEvent& event) override;
    };