#include "UIArea.h"

SDLSkiaWindow* UIArea::getWindow()
    {
    if (!window && parent)
        window = parent->getWindow();
    return window;
    }

View* UIArea::getView()
    {
    if (!view && parent)
        view = parent->getView();
    return view;
    }

void UIArea::drawAll()
    {
    getWindow()->Canvas().save();
    getWindow()->Canvas().translate(rect.fLeft, rect.fTop);
    totalTransform = getWindow()->Canvas().getTotalMatrix();
    _drawMe();
    getWindow()->Canvas().restore();
    }

void UIArea::trickleResizeEvent()
    {
    if (resize)
        resize(*this);
    _resizeContent();
    }

bool UIArea::trickleMouseMoveEvent(SDL_MouseMotionEvent& event)
    {
    if (!hitTest((SkScalar)event.x, (SkScalar)event.y))
        return false;
    _mouseMove(event);
    if (mouseMove)
        mouseMove(*this, event);
    return true;
    }

bool UIArea::trickleMouseUpEvent(SDL_MouseButtonEvent& event)
    {
    if (!hitTest((SkScalar)event.x, (SkScalar)event.y))
        return false;
    if (!getWindow()->isMouseCaptured(*this))
        {
        _mouseUp(event);
        if (mouseUp)
            mouseUp(*this, event);
        }
    return true;
    }

bool UIArea::trickleMouseDownEvent(SDL_MouseButtonEvent& event)
    {
    if (!hitTest((SkScalar)event.x, (SkScalar)event.y))
        return false;
    _mouseDown(event);
    if (mouseDown)
        mouseDown(*this, event);
    return true;
    }

void UIArea::mapPixelsToPoints(SkPoint* points, int count)
    {
    SkMatrix invertedMatrix;
    invertedMatrix.reset();
    totalTransform.invert(&invertedMatrix);
    invertedMatrix.mapPoints(points, 1);
    }

void UIArea::mapPointsToPixels(SkPoint* dst, SkPoint* src, int count)
    {
    totalTransform.mapPoints(dst, src, 1);
    }

bool UIArea::hitTest(SkScalar x, SkScalar y)
    {
    SkPoint mouse = SkPoint::Make(x, y);
    if (parent)
        parent->mapPixelsToPoints(&mouse, 1);
    return rect.contains(mouse.fX, mouse.fY);
    }

SkRect UIArea::absoluteRect()
    {
    SkScalar xOffset, yOffset;
    xOffset = 0;
    yOffset = 0;
    UIArea* p = parent;
    while (p)
        {
        xOffset += p->rect.left();
        yOffset += p->rect.top();
        p = p->parent;
        }
    SkRect rectAbs = rect;
    rectAbs.offset(xOffset, yOffset);
    return rectAbs;
    }

