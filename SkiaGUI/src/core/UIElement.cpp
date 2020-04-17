#include "UIElement.h"

void UIElement::drawAll(SDLSkiaWindow& window)
    {
    window.Canvas().save();
    window.Canvas().translate(rect.fLeft, rect.fTop);
    drawBackground(window);
    totalTransform = window.Canvas().getTotalMatrix();
    drawMe(window);
    drawChildren(window);
    window.Canvas().restore();
    }

void UIElement::drawChildren(SDLSkiaWindow& window)
    {
    for (auto el : children)
        el->drawAll(window);
    }

UIElement& UIElement::operator+=(UIElement* child)
    {
    children.push_back(child);
    child->parent = this;
    return *this;
    }

void UIElement::trickleResizeEvent(SDL_WindowEvent& event, SDLSkiaWindow& window)
    {
    if(resize)
        resize(*this, event, window);
    for (auto el : children)
        {
            el->trickleResizeEvent(event, window);
        }
    }

bool UIElement::trickleMouseMoveEvent(SDL_MouseMotionEvent& event, SDLSkiaWindow& window)
    {
    if (!hitTest((SkScalar)event.x, (SkScalar)event.y))
        return false;
    _mouseMove(event, window);
    if (mouseMove)
        mouseMove(*this, event, window);
    for (auto el : children)
        {
        if (el->trickleMouseMoveEvent(event, window))
            break; //assuming no overlapping UIElements.
        }
    return true;
    }

bool UIElement::trickleMouseUpEvent(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    if (!hitTest((SkScalar)event.x, (SkScalar)event.y))
        return false;
    _mouseUp(event, window);
    if (mouseUp)
        mouseUp(*this, event, window);
    for (auto el : children)
        {
        if (el->trickleMouseUpEvent(event, window))
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

void UIElement::drawBackground(SDLSkiaWindow& window)
    {
    if (backgroundColor != SK_ColorTRANSPARENT)
        {
        SkPaint paint;
        paint.setColor(backgroundColor);
        paint.setStyle(SkPaint::Style::kFill_Style);
        window.Canvas().drawRect(SkRect::MakeWH(rect.width(), rect.height()), paint);
        }
    }
