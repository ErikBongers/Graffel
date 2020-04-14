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

void InfiniteCanvas::_mouseMove(SDL_MouseMotionEvent& event, SDLSkiaWindow& window)
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

void InfiniteCanvas::_mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    isDragging = false;
    }

void InfiniteCanvas::mouseWheel(SDL_MouseWheelEvent& event, SDLSkiaWindow& window)
    {
    auto state = SDL_GetMouseState(nullptr, nullptr);
    if (SDL_BUTTON(SDL_BUTTON_MIDDLE) & state)
        return; //accidentally scrolled while pressing the middle (wheel) button

    int x, y;
    SDL_GetMouseState(&x, &y);

    SkPoint mouseLoc;
    SkPoint canvasLoc;
    mouseLoc.set((SkScalar)x, (SkScalar)y);
    mapPixelsToPoints(&canvasLoc, &mouseLoc, 1);

    if (event.y > 0)
        scaleFactor *= 1 + scaleSpeed;
    else
        scaleFactor *= 1 - scaleSpeed;

    SkPoint newMouseLoc;
    mapPointsToPixels(&newMouseLoc, &canvasLoc, 1);
    xTranslate += mouseLoc.fX - newMouseLoc.fX;
    yTranslate += mouseLoc.fY - newMouseLoc.fY;

    window.setInvalid();
    }

SkMatrix InfiniteCanvas::createMatrix()
    {
    SkMatrix matrix;
    matrix.reset();
    matrix = matrix.setTranslate(xTranslate, yTranslate)
        .preScale(scaleFactor, scaleFactor);
    return matrix;
    }

void InfiniteCanvas::mapPixelsToPoints(SkPoint* dst, SkPoint* src, int count)
    {
    SkMatrix invertedMatrix;
    invertedMatrix.reset();
    createMatrix().invert(&invertedMatrix);
    invertedMatrix.mapPoints(dst, src, 1);
    }

void InfiniteCanvas::mapPointsToPixels(SkPoint* dst, SkPoint* src, int count)
    {
    createMatrix().mapPoints(dst, src, 1);
    }

