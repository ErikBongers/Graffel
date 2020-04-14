#include "Button.h"

Button::Button()
    {
    mouseMove = [](UIElement& e, SDL_MouseMotionEvent& event, SDLSkiaWindow& window) {
        Button& button = (Button&)e;
        if(button.hitTest(event.x, event.y))
            button.highlight(true, window);
        else
            {
            button.highlight(false, window);
            window.removeMouseCapture(button);
            }
        };
    }

void Button::highlight(bool high, SDLSkiaWindow& window)
    {
    if (highlightColor == SK_ColorTRANSPARENT)
        return;
    if (high)
        {
        if (backgroundColor != highlightColor)
            {
            originalBkcColor = backgroundColor;
            backgroundColor = highlightColor;
            window.addMouseCapture(*this);
            window.setInvalid();
            }
        }
    else
        {
        if (backgroundColor != originalBkcColor)
            {
            backgroundColor = originalBkcColor;
            window.setInvalid();
            }
        }
    }
