#include "pch.h"
#include "InfiniteCanvas.h"

void InfiniteCanvas::drawAll(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window)
    {
    drawBackground(xOffset, yOffset, window);
    drawMe(xOffset, yOffset, window);
    window.Canvas().save();
    window.Canvas().clipRect(absoluteRect());
    window.Canvas().translate(xTranslate, yTranslate);
    drawChildren(xOffset, yOffset, window);
    window.Canvas().restore();
    }

void InfiniteCanvas::mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window)
    {
    if (isDragging)
        {
        xTranslate = startDragTranslate.fX + event.x - startDrag.fX;
        yTranslate = startDragTranslate.fY + event.y - startDrag.fY;
        }
    }

void InfiniteCanvas::mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    if (isDragging)
        return;
    if (event.state == SDL_PRESSED && event.button == SDL_BUTTON_MIDDLE)
        {
        startDrag.set((SkScalar)event.x, (SkScalar)event.y);
        startDragTranslate.set(xTranslate, yTranslate);
        isDragging = true;
        }
    }

void InfiniteCanvas::mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    isDragging = false;
    }
