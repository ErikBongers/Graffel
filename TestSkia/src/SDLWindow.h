#pragma once
#include "pch.h"
#include "graffel\Timeline.h"
#include "graffel\Tick.h"
#include "graffel\Block.h"
#include "Fps.h"

class SDLWindow
    {
    private:
        bool quit = false;
        SDL_Window* window = NULL;
        int dw, dh;
        GrGLint buffer;
        GrGLFramebufferInfo info;
        SkColorType colorType;
        sk_sp<SkSurface> surface;
        SkCanvas* canvas;
        int rotation = 0;
        SkFont font;
        graffel::Block b;
        graffel::Fps fps;
        SkPaint paint;
        SDL_GLContext glContext = nullptr;
        uint32_t windowFormat;
        sk_sp<const GrGLInterface> interfac;
        graffel::Timeline* masterTimeline;
        int contextType;
        int stencilBits;
        int msaaSampleCount;
        sk_sp<GrContext> grContext;

        SkCanvas* createSurfaceAndCanvas(sk_sp<const GrGLInterface> interfac, uint32_t windowFormat, int contextType, sk_sp<GrContext> grContext);
        void handleEvents();
        void resizeViewportToWindow(SDL_Window* window);

    public:
        void startEventLoop();
        bool createWindow(int width, int height, int stencilBits, int msaaSampleCount);
        void destroyWindow();
    };

