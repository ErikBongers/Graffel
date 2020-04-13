#include "UIElement.h"

void UIElement::drawAll(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window)
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
    drawMe(xOffset, yOffset, window); 
    for (auto it = children.begin(); it < children.end(); it++)
        (*it)->drawAll(rect.left() + xOffset, rect.top() + yOffset, window);
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
    for (auto it = children.begin(); it < children.end(); it++)
        {
            (*it)->trickleResizeEvent(event, window);
        }
    }
