#include "pch.h"
#include "SDLSkiaWindow.h"
#include "UIElement.h"


class GraffelWindowClient : public WindowClient
    {
    private:
        graffel::Block b;
        graffel::Timeline* masterTimeline;
        UIElement full;
    public:
        void initialize(SDLSkiaWindow& window) override;
        void update(SDLSkiaWindow& window) override;
        void draw(SDLSkiaWindow& window) override;
        void mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window) override;
        void mouseClicked(SDL_MouseMotionEvent& event, SDLSkiaWindow& window) override;
        void resize(SDL_WindowEvent& event, SDLSkiaWindow& window) override;
    };