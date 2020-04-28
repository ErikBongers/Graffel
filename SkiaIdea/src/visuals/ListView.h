#pragma once
#include "../pch.h"
#include "ImageTextButton.h"

class ListView : public UIElement
    {
    protected:
        void _resize(SDL_WindowEvent& event) override;
    };