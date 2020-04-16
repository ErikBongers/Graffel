#include "pch.h"
#include "core/SDLSkiaWindow.h"
#include "controls/Button.h"
#include "controls/InfiniteCanvas.h"

class Bullet : public UIElement
    {
    void drawMe(SkScalar xOffset, SkScalar yOffset, SDLSkiaWindow& window) override
        {
        SkPaint paint;
        paint.setColor(SK_ColorRED);
        SkScalar radius = rect.width() / 2;
        window.Canvas().drawCircle(SkPoint::Make(xOffset+ radius, yOffset+ radius), radius, paint);
        }
    };


class TestWindowClient : public WindowClient
    {
    private:
        UIElement full;
        UIElement toolbar;
        Button button1;
        Button button2;
        InfiniteCanvas infiniteCanvas;
        UIElement square1;
        Bullet bullet;

    public:
        void initialize(SDLSkiaWindow& window) override;
        void update(SDLSkiaWindow& window) override;
        void draw(SDLSkiaWindow& window) override;
        void mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window) override;
        void mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
        void mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
        void mouseWheel(SDL_MouseWheelEvent& event, SDLSkiaWindow& window) override;
        void resize(SDL_WindowEvent& event, SDLSkiaWindow& window) override;
    };