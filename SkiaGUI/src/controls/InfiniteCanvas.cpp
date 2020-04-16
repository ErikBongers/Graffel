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
        if (dragee == this)
            {
            xTranslate = dragStartDrageePos.fX + event.x - dragStartMousePos.fX;
            yTranslate = dragStartDrageePos.fY + event.y - dragStartMousePos.fY;
            }
        else
            {
            SkScalar newDrageePosX = dragStartDrageePos.fX + event.x - dragStartMousePos.fX;
            SkScalar newDrageePosY = dragStartDrageePos.fY + event.y - dragStartMousePos.fY;
            //convert newDrageePos to Points
            SkPoint newDrageePos = SkPoint::Make(newDrageePosX, newDrageePosY);
            SkPoint newDrageePoint;
            mapPixelsToPoints(&newDrageePoint, &newDrageePos, 1);
            dragee->rect = SkRect::MakeXYWH(newDrageePoint.fX, newDrageePoint.fY, dragee->rect.width(), dragee->rect.height());
            }
        window.setInvalid();
        }
    SkPoint mouseLoc;
    SkPoint canvasLoc;
    mouseLoc.set((SkScalar)event.x, (SkScalar)event.y);
    mapPixelsToPoints(&canvasLoc, &mouseLoc, 1);
    std::cout << canvasLoc.fX << ", " << canvasLoc.fY << std::endl;
    }

void InfiniteCanvas::mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    if (isDragging)
        return;
    SkPoint mouse = SkPoint::Make((SkScalar)event.x, (SkScalar)event.y);
    SkPoint point;
    mapPixelsToPoints(&point, &mouse, 1);
    for (std::vector<UIElement*>::reverse_iterator it = children.rbegin(); it != children.rend(); ++it)
        {
        UIElement* e = *it;
        if (e->hitTest(point.fX, point.fY, true))
            {
            //start drag of object
            //TODO: ask first
            dragee = e;
            dragStartMousePos.set((SkScalar)event.x, (SkScalar)event.y);
            SkPoint p = SkPoint::Make(e->rect.fLeft, e->rect.fTop);
            mapPointsToPixels(&dragStartDrageePos, &p, 1);

            isDragging = true;
            return;
            }
        }
    if (event.state == SDL_PRESSED && event.button == SDL_BUTTON_MIDDLE)
        {
        dragee = this;
        dragStartMousePos.set((SkScalar)event.x, (SkScalar)event.y);
        dragStartDrageePos.set(xTranslate, yTranslate);
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
    SkRect abs = absoluteRect();
    SkMatrix mat = createMatrix();
    mat.preTranslate(abs.fLeft, abs.fTop);
    mat.invert(&invertedMatrix);
    invertedMatrix.mapPoints(dst, src, 1);
    }

void InfiniteCanvas::mapPointsToPixels(SkPoint* dst, SkPoint* src, int count)
    {
    SkRect abs = absoluteRect();
    SkMatrix mat = createMatrix();
    mat.preTranslate(abs.fLeft, abs.fTop);
    mat.mapPoints(dst, src, 1);
    }
