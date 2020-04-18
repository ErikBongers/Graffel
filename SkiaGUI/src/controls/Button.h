#include "../pch.h"
#include "../core/UIElement.h"

class Button : public UIElement
    {
    protected:
        SkColor originalBkcColor = SK_ColorTRANSPARENT;
        bool mouseOver = false;
        void highlight(bool high, SDLSkiaWindow& window);
    public:
        SkColor highlightColor = SK_ColorTRANSPARENT;
        Button();
    };