#include "Button.h"

Button::Button()
    {
    mouseMove = [](UIElement& e, SDL_MouseMotionEvent& event, SDLSkiaWindow& window) {
        Button& button = (Button&)e;
        button.highlight(button.hitTest((SkScalar)event.x, (SkScalar)event.y), window);
        };
    }

void Button::highlight(bool high, SDLSkiaWindow& window)
    {
    if (high)
        {
        if (!mouseOver)
            {
            mouseOver = true;
            window.addMouseCapture(*this);
            originalBkcColor = backgroundColor;
            if (highlightColor != SK_ColorTRANSPARENT)
                backgroundColor = highlightColor;
            window.setInvalid();
            }
        }
    else
        {
        if (mouseOver)
            {
            mouseOver = false;
            backgroundColor = originalBkcColor;
            window.setInvalid();
            }
        window.removeMouseCapture(*this);
        }
    }
