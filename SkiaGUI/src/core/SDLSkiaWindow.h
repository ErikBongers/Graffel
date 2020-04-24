#pragma once
#include "pch.h"
#include "Fps.h"
#include <set>

class SDLSkiaWindow;
class UIElement;
class WindowClient
    {
    protected:
        SDLSkiaWindow* window;
        void setWindow(SDLSkiaWindow* window) { this->window = window; }
    public:
        SDLSkiaWindow* getWindow() { return window; }
        virtual void onIdle() {}
        virtual void draw() {}
        virtual void initialize() {}
        virtual void mouseMoved(SDL_MouseMotionEvent& event) {}
        virtual void mouseDown(SDL_MouseButtonEvent& event) {}
        virtual void mouseUp(SDL_MouseButtonEvent& event) {}
        virtual void mouseWheel(SDL_MouseWheelEvent& event) {}
        virtual void keyDown(SDL_KeyboardEvent& event) {}
        virtual void textInput(SDL_TextInputEvent& event) {}
        virtual void resize(SDL_WindowEvent& event) {}
        virtual UIElement* getRootElement() = 0;
        friend class SDLSkiaWindow;
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
        std::set<UIElement*> mouseCaptures;
        UIElement* root = nullptr;
    public:
        SDLSkiaWindow(WindowClient& client) : client(client) { client.setWindow(this); root = client.getRootElement(); }
        void startEventLoop();
        bool createWindow(int width, int height, int stencilBits, int msaaSampleCount);
        void destroyWindow();
        int getWidth() { return dw; }
        int getHeight() { return dh; }
        SkCanvas& Canvas() { return *canvas; }
        void setInvalid() { invalid = true; }
        void addMouseCapture(UIElement& e);
        void removeMouseCapture(UIElement& e);
        bool isMouseCaptured(UIElement& e) { return mouseCaptures.find(&e) != mouseCaptures.end(); }
        void close() { quit = true; }
    };

