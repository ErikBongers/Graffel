#include "pch.h"
#include "InfiniteCanvas.h"

void InfiniteCanvas::drawAll(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window)
    {
    drawBackground(xOffset, yOffset, window);
    drawMe(xOffset, yOffset, window);
    window.Canvas().save();
    window.Canvas().clipRect(absoluteRect());
    window.Canvas().translate(xTranslate, yTranslate);
    window.Canvas().scale(scaleFactor, scaleFactor); //pre scaling!
    drawChildren(xOffset, yOffset, window);
    window.Canvas().restore();
    }

void InfiniteCanvas::mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window)
    {
    if (isDragging)
        {
        xTranslate = startDragTranslate.fX + event.x - startDrag.fX;
        yTranslate = startDragTranslate.fY + event.y - startDrag.fY;
        window.setInvalid();
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

void InfiniteCanvas::mouseWheel(SDL_MouseWheelEvent& event, SDLSkiaWindow& window)
    {
    auto state = SDL_GetMouseState(nullptr, nullptr);
    if (SDL_BUTTON(SDL_BUTTON_MIDDLE) & state)
        return; //accidentally scrolled while pressing the middle (wheel) button
    SkMatrix matrix;
    matrix.reset();
    matrix = matrix.setTranslate(xTranslate, yTranslate);
    matrix = matrix.preScale(scaleFactor, scaleFactor);
    SkMatrix invertedMatrix;
    invertedMatrix.reset();
    matrix.invert(&invertedMatrix);
    SkPoint mouseLoc;
    SkPoint canvasLoc;
    int x, y;
    SDL_GetMouseState(&x, &y);
    mouseLoc.set((SkScalar)x, (SkScalar)y);
    invertedMatrix.mapPoints(&canvasLoc, &mouseLoc, 1);

    if (event.y > 0)
        scaleFactor *= 1 + scaleSpeed;
    else
        scaleFactor *= 1 - scaleSpeed;

    matrix.reset();
    matrix = matrix.setTranslate(xTranslate, yTranslate);
    matrix = matrix.preScale(scaleFactor, scaleFactor);
    SkPoint newMouseLoc;
    matrix.mapPoints(&newMouseLoc, &canvasLoc, 1);
    xTranslate += mouseLoc.fX - newMouseLoc.fX;
    yTranslate += mouseLoc.fY - newMouseLoc.fY;

    window.setInvalid();
    }
