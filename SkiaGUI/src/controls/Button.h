#include "../pch.h"
#include "../core/UIElement.h"

class Button : public UIElement
    {
    private:
        SkColor originalBkcColor = SK_ColorTRANSPARENT;
    public:
        SkColor highlightColor = SK_ColorTRANSPARENT;
        Button();
        void highlight(bool high, SDLSkiaWindow& window);
    };