#include "pch.h"
#include "core/SDLSkiaWindow.h"
#include "controls/Button.h"
#include "controls/InfiniteCanvas.h"


class TestWindowClient : public WindowClient
    {
    private:
        UIElement full;
        UIElement toolbar;
        Button button1;
        Button button2;
        InfiniteCanvas infiniteCanvas;
        UIElement square1;

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