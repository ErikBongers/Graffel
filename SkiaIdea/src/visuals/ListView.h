#pragma once
#include "../pch.h"
#include "ImageTextButton.h"

class ListView : public UIElement
    {
    protected:
        void _resizeContent(SDL_WindowEvent& event) override;
    };