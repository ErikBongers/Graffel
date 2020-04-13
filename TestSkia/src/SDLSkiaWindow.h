#pragma once
#include "pch.h"
#include "graffel\Timeline.h"
#include "graffel\Tick.h"
#include "graffel\Block.h"
#include "Fps.h"

class SDLSkiaWindow;
class WindowClient
    {
    public:
        virtual void update(SDLSkiaWindow& window) {}
        virtual void draw(SDLSkiaWindow& window) {}
        virtual void initialize(SDLSkiaWindow& window) {}
        virtual void mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window) {}
        virtual void mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) {}
        virtual void mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window) {}
        virtual void mouseWheel(SDL_MouseWheelEvent& event, SDLSkiaWindow& window) {}
        virtual void resize(SDL_WindowEvent& event, SDLSkiaWindow& window) {}
    };

class SDLSkiaWindow
    {
    private:
        WindowClient& client;
        bool quit = false;
        SDL_Window* window = nullptr;
        int dw = 0, dh = 0;
        GrGLint buffer = 0;
        GrGLFramebufferInfo info;
        SkColorType colorType = SkColorType::kUnknown_SkColorType;
        sk_sp<SkSurface> surface;
        SkCanvas* canvas = nullptr;
        int rotation = 0;
        SkFont font;
        graffel::Fps fps;
        SkPaint paint;
        SDL_GLContext glContext = nullptr;
        uint32_t windowFormat = 0;
        sk_sp<const GrGLInterface> interfac;
        int contextType = 0;
        int stencilBits = 0;
        int msaaSampleCount = 0;
        sk_sp<GrContext> grContext;
        bool invalid = true;
        int loopCounter = 0;

        SkCanvas* createSurfaceAndCanvas(sk_sp<const GrGLInterface> interfac, uint32_t windowFormat, int contextType, sk_sp<GrContext> grContext);
        bool handleEvents();
        void resizeViewportToWindow(SDL_Window* window);
        void loopOnce();
        static int onEventsReceived(void* userdata, SDL_Event* event);

    public:
        SDLSkiaWindow(WindowClient& client) : client(client) {}
        void startEventLoop();
        bool createWindow(int width, int height, int stencilBits, int msaaSampleCount);
        void destroyWindow();
        int getWidth() { return dw; }
        int getHeight() { return dh; }
        SkCanvas& Canvas() { return *canvas; }
        void setInvalid() { invalid = true; }
    };

