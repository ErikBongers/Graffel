#include "UIElement.h"
#include "UIArea.h"

void UIElement::_drawMe()
    {
    drawBackground();
    drawMe();
    drawChildren();
    }

void UIElement::drawChildren()
    {
    for (auto el : children)
        el->drawAll();
    }

UIElement& UIElement::operator+=(UIArea* child)
    {
    children.push_back(child);
    child->parent = this;
    return *this;
    }


void UIElement::trickleIdle() 
    { 
    onIdle();  
    for (auto c : children) 
        c->trickleIdle(); 
    }

void UIElement::trickleKeyDown(SDL_KeyboardEvent& event) 
    { 
    keyDown(event);  
    for (auto c : children) 
        c->trickleKeyDown(event); 
    }

void UIElement::trickleTextEvent(SDL_TextInputEvent& event) 
    { 
    textInput(event);
    for (auto c : children) 
        c->trickleTextEvent(event); 
    }

void UIElement::trickleResizeEvent(SDL_WindowEvent& event) 
    { 
    UIArea::trickleResizeEvent(event); 
    for (auto el : children) 
        el->trickleResizeEvent(event); 
    }

bool UIElement::trickleMouseMoveEvent(SDL_MouseMotionEvent& event) 
    { 
    if (!UIArea::trickleMouseMoveEvent(event))
        return false;
    for (auto el : children)
        if (el->trickleMouseMoveEvent(event))
            return true;
    return false; 
    }

bool UIElement::trickleMouseUpEvent(SDL_MouseButtonEvent& event) 
    { 
   if(!UIArea::trickleMouseUpEvent(event))
      return false;
    for (auto el : children)
        if (el->trickleMouseUpEvent(event))
            return true; 
    return false;
    }

bool UIElement::trickleMouseDownEvent(SDL_MouseButtonEvent& event) 
    { 
    if(!UIArea::trickleMouseDownEvent(event))
        return false;
    for (auto el : children)
        if (el->trickleMouseDownEvent(event))
            return true;
    return false; 
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
