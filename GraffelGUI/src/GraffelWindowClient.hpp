#include "pch.h"
#include "SDLSkiaWindow.h"
#include "UIElement.h"
#include "InfiniteCanvas.h"


class GraffelWindowClient : public WindowClient
    {
    private:
        graffel::Block b;
        graffel::Timeline* masterTimeline;


        UIElement full;
        UIElement toolbar;
        UIElement button1;
        UIElement button2;
        InfiniteCanvas infiniteCanvas;
        UIElement square1;

    public:
        void initialize(SDLSkiaWindow& window) override;
        void update(SDLSkiaWindow& window) override;
        void draw(SDLSkiaWindow& window) override;
        void mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window) override;
        void mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
        void mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) override;
        void mouseWheel(SDL_MouseWheelEvent_EX& event, SDLSkiaWindow& window) override;
        void resize(SDL_WindowEvent& event, SDLSkiaWindow& window) override;
    };