#include "UIElement.h"

SDLSkiaWindow* UIElement::getWindow()
    {
    if (!window)
        {
        if(parent)
            window = parent->getWindow();
        }
    return window;
    }

void UIElement::drawAll()
    {
    getWindow()->Canvas().save();
    getWindow()->Canvas().translate(rect.fLeft, rect.fTop);
    drawBackground();
    totalTransform = getWindow()->Canvas().getTotalMatrix();
    drawMe();
    drawChildren();
    getWindow()->Canvas().restore();
    }

void UIElement::drawChildren()
    {
    for (auto el : children)
        el->drawAll();
    }

UIElement& UIElement::operator+=(UIElement* child)
    {
    children.push_back(child);
    child->parent = this;
    return *this;
    }

void UIElement::trickleResizeEvent(SDL_WindowEvent& event)
    {
    if(resize)
        resize(*this, event);
    _resize(event);
    for (auto el : children)
        {
            el->trickleResizeEvent(event);
        }
    }

bool UIElement::trickleMouseMoveEvent(SDL_MouseMotionEvent& event)
    {
    if (!hitTest((SkScalar)event.x, (SkScalar)event.y))
        return false;
    _mouseMove(event);
    if (mouseMove)
        mouseMove(*this, event);
    for (auto el : children)
        {
        if (el->trickleMouseMoveEvent(event))
            break; //assuming no overlapping UIElements.
        }
    return true;
    }

bool UIElement::trickleMouseUpEvent(SDL_MouseButtonEvent& event)
    {
    if (!hitTest((SkScalar)event.x, (SkScalar)event.y))
        return false;
    if (!getWindow()->isMouseCaptured(*this))
        {
        _mouseUp(event);
        if (mouseUp)
            mouseUp(*this, event);
        }
    for (auto el : children)
        {
        if (el->trickleMouseUpEvent(event))
            break; //assuming no overlapping UIElements.
        }
    return true;
    }

bool UIElement::trickleMouseDownEvent(SDL_MouseButtonEvent& event)
    {
    if (!hitTest((SkScalar)event.x, (SkScalar)event.y))
        return false;
    _mouseDown(event);
    if (mouseDown)
        mouseDown(*this, event);
    for (auto el : children)
        {
        if (el->trickleMouseDownEvent(event))
            break; //assuming no overlapping UIElements.
        }
    return true;
    }

void UIElement::mapPixelsToPoints(SkPoint* points, int count)
    {
    SkMatrix invertedMatrix;
    invertedMatrix.reset();
    totalTransform.invert(&invertedMatrix);
    invertedMatrix.mapPoints(points, 1);
    }

void UIElement::mapPointsToPixels(SkPoint* dst, SkPoint* src, int count)
    {
    totalTransform.mapPoints(dst, src, 1);
    }

bool UIElement::hitTest(SkScalar x, SkScalar y)
    {
    SkPoint mouse = SkPoint::Make(x, y);
    if(parent)
        parent->mapPixelsToPoints(&mouse, 1);
    return rect.contains(mouse.fX, mouse.fY);
    }

SkRect UIElement::absoluteRect()
    {
    SkScalar xOffset, yOffset;
    xOffset = 0;
    yOffset = 0;
    UIElement* p = parent;
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

void UIElement::drawBackground()
    {
    if (backgroundColor != SK_ColorTRANSPARENT)
        {
        SkPaint paint;
        paint.setColor(backgroundColor);
        paint.setStyle(SkPaint::Style::kFill_Style);
        getWindow()->Canvas().drawRect(SkRect::MakeWH(rect.width(), rect.height()), paint);
        }
    }
