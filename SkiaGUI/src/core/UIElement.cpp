#include "UIElement.h"

void UIElement::drawAll(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window)
    {
    drawBackground(xOffset, yOffset, window);
    drawMe(xOffset + rect.fLeft, yOffset + rect.fTop, window);
    drawChildren(xOffset, yOffset, window);
    }

void UIElement::drawChildren(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window)
    {
    for (auto el : children)
        el->drawAll(rect.left() + xOffset, rect.top() + yOffset, window);
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
    if (!hitTest(event.x, event.y))
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
    if (!hitTest(event.x, event.y))
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

bool UIElement::hitTest(int x, int y)
    {
    return absoluteRect().contains((SkScalar)x, (SkScalar)y);
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

void UIElement::drawBackground(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window)
    {
    if (backgroundColor != SK_ColorTRANSPARENT)
        {
        SkPaint paint;
        paint.setColor(backgroundColor);
        paint.setStyle(SkPaint::Style::kFill_Style);
        SkRect absRect = rect;
        absRect.offset(xOffset, yOffset);
        window.Canvas().drawRect(absRect, paint);
        }
    }
