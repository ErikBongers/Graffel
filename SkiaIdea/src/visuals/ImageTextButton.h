#pragma once
#include "../pch.h"
#include "controls/Button.h"

class ImageTextButton : public Button
    {
    public:
        ImageTextButton() { highlightColor = SkColorSetARGB(20, 255, 255, 255); }
        SkScalar xOffset = 4, yOffset = 4;
        sk_sp<SkImage> img = nullptr;
        std::string text;
        void drawMe() override
            {
            Button::drawMe();
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setAlpha(mouseOver ? 255 : 180);
            paint.setColor({ 1,1,1,1 });
            if (img)
                Canvas().drawImage(img, xOffset, yOffset, &paint);
            if (!text.empty())
                {
                SkFont font;
                Canvas().drawString(text.c_str(), xOffset + 25, yOffset + 15, font, paint);
                }
            }

    };