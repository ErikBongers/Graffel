#include "pch.h"
#include "InfiniteCanvas.h"

void InfiniteCanvas::_drawMe()
    {
    baseTransform = getWindow()->Canvas().getTotalMatrix();
    drawBackground();
    drawMe();
    getWindow()->Canvas().clipRect(SkRect::MakeWH(rect.width(), rect.height()));
    getWindow()->Canvas().translate(xTranslate, yTranslate);
    getWindow()->Canvas().scale(scaleFactor, scaleFactor); //pre scaling!
    totalTransform = getWindow()->Canvas().getTotalMatrix();
    drawChildren();
    }

void InfiniteCanvas::_mouseMove(SDL_MouseMotionEvent& event)
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
            SkPoint newDrageePoint = newDrageePos;
            mapPixelsToPoints(&newDrageePoint, 1);
            dragee->rect = SkRect::MakeXYWH(newDrageePoint.fX, newDrageePoint.fY, dragee->rect.width(), dragee->rect.height());
            }
        getWindow()->setInvalid();
        }
    }

void InfiniteCanvas::_mouseDown(SDL_MouseButtonEvent& event)
    {
    if (isDragging)
        return;
    for (auto it = children.rbegin(); it != children.rend(); ++it)
        {
        auto e = *it;
        if (e->hitTest((SkScalar)event.x, (SkScalar)event.y))
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

void InfiniteCanvas::_mouseUp(SDL_MouseButtonEvent& event)
    {
    isDragging = false;
    }

void InfiniteCanvas::mouseWheel(SDL_MouseWheelEvent& event)
    {
    auto state = SDL_GetMouseState(nullptr, nullptr);
    if (SDL_BUTTON(SDL_BUTTON_MIDDLE) & state)
        return; //accidentally scrolled while pressing the middle (wheel) button

    int x, y;
    SDL_GetMouseState(&x, &y);

    SkPoint mouseLoc;
    SkPoint canvasLoc;
    mouseLoc.set((SkScalar)x, (SkScalar)y);
    canvasLoc = mouseLoc;
    mapPixelsToPoints(&canvasLoc, 1);

    if (event.y > 0)
        scaleFactor *= 1 + scaleSpeed;
    else
        scaleFactor *= 1 - scaleSpeed;

    SkPoint newMouseLoc;
    totalTransform = baseTransform.preTranslate(xTranslate, yTranslate)
                                  .preScale(scaleFactor, scaleFactor);
    mapPointsToPixels(&newMouseLoc, &canvasLoc, 1);
    xTranslate += mouseLoc.fX - newMouseLoc.fX;
    yTranslate += mouseLoc.fY - newMouseLoc.fY;

    getWindow()->setInvalid();
    }

