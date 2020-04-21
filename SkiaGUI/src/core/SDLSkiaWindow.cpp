#include "SDLSkiaWindow.h"
#include "Functions.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "UIElement.h"

int SDLCALL SDLSkiaWindow::onEventsReceived(void* userdata, SDL_Event* event)
    {
    ((SDLSkiaWindow*)userdata)->loopOnce();
    return 0;//ignored
    }

void SDLSkiaWindow::startEventLoop() 
    {
    //SDL_AddEventWatch(onEventsReceived, (void*)this);

    while (!quit) { // Our application loop
        loopOnce();
        Sleep(10);
        }
    }

void SDLSkiaWindow::loopOnce()
    {
    loopCounter++;

    if (!handleEvents())
        client.onIdle(*this);
    if (canvas == NULL)
        canvas = createSurfaceAndCanvas(interfac, windowFormat, contextType, grContext);

    if (invalid)
        {
        fps.beginFrame();
        canvas->restoreToCount(0);
        canvas->resetMatrix();
        canvas->clear(SK_ColorWHITE);

        client.draw(*this);

        std::string strFps = std::to_string(fps.getFps());
        std::string strMs = std::to_string((int)fps.getMsPerFrame()) + "ms";
        std::string strLoopCnt = std::to_string(loopCounter) + "loop iterations";
        paint.setColor(SK_ColorWHITE); // move to init function?
        canvas->drawString(strFps.c_str(), 100.0f, 160.0f, font, paint);
        canvas->drawString(strMs.c_str(), 100.0f, 180.0f, font, paint);
        canvas->drawString(strLoopCnt.c_str(), 100.0f, 200.0f, font, paint);

        fps.endFrame();

        canvas->flush();
        SDL_GL_SwapWindow(window);
        invalid = false;
        }
    }

bool  SDLSkiaWindow::createWindow(int width, int height, int stencilBits, int msaaSampleCount)
    {
    this->stencilBits = stencilBits;
    this->msaaSampleCount = msaaSampleCount;
    window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        handle_error();
        return false;
        }

    // To go fullscreen
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // try and setup a GL context
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        handle_error();
        return false;
        }

    int success = SDL_GL_MakeCurrent(window, glContext);
    if (success != 0) {
        handle_error();
        return false;
        }

    windowFormat = SDL_GetWindowPixelFormat(window);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);


    resizeViewportToWindow(window);
    glClearColor(1, 1, 1, 1);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //SKIA SETUP starts here...

    // setup GrContext
    interfac = GrGLMakeNativeInterface();

    // setup contexts
    grContext = GrContext::MakeGL(interfac);
    SkASSERT(grContext);

    canvas = createSurfaceAndCanvas(interfac, windowFormat, contextType, grContext);
    //canvas->scale((float)dw/dm.w, (float)dh/dm.h); //scales to about 1:1

    client.initialize(*this);
    SDL_WindowEvent event;
    event.type = SDL_EventType::SDL_WINDOWEVENT;
    client.resize(event, *this);
    return true;
    }

void SDLSkiaWindow::destroyWindow()
    {
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
        }

    SDL_DestroyWindow(window);
    }

void SDLSkiaWindow::addMouseCapture(UIElement& e)
    {
    mouseCaptures.insert(&e);
    }

void SDLSkiaWindow::removeMouseCapture(UIElement& e)
    {
    mouseCaptures.extract(&e);
    }


SkCanvas* SDLSkiaWindow::createSurfaceAndCanvas(sk_sp<const GrGLInterface> interfac, uint32_t windowFormat, int contextType, sk_sp<GrContext> grContext)
    {
    // Wrap the frame buffer object attached to the screen in a Skia render target so Skia can render to it
    GR_GL_GetIntegerv(interfac.get(), GR_GL_FRAMEBUFFER_BINDING, &buffer);
    info.fFBOID = (GrGLuint)buffer;

    //SkDebugf("%s", SDL_GetPixelFormatName(windowFormat));
    // TODO: the windowFormat is never any of these?
    if (SDL_PIXELFORMAT_RGBA8888 == windowFormat) {
        info.fFormat = GR_GL_RGBA8;
        colorType = kRGBA_8888_SkColorType;
        }
    else {
        colorType = kBGRA_8888_SkColorType;
        if (SDL_GL_CONTEXT_PROFILE_ES == contextType) {
            info.fFormat = GR_GL_BGRA8;
            }
        else {
            // We assume the internal format is RGBA8 on desktop GL
            info.fFormat = GR_GL_RGBA8;
            }
        }

    // setup SkSurface
    // To use distance field text, use commented out SkSurfaceProps instead
    // SkSurfaceProps props(SkSurfaceProps::kUseDeviceIndependentFonts_Flag, SkSurfaceProps::kLegacyFontHost_InitType);
    SkSurfaceProps props(SkSurfaceProps::kLegacyFontHost_InitType);

    GrBackendRenderTarget target(dw, dh, msaaSampleCount, stencilBits, info);
    surface = SkSurface::MakeFromBackendRenderTarget(grContext.get(), target, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, &props);

    return surface->getCanvas();
    }

bool SDLSkiaWindow::handleEvents()
    {
    bool eventsHappened = false;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        eventsHappened = true;
        switch (event.type) {
            case SDL_MOUSEMOTION:
                for (auto el = mouseCaptures.rbegin(); el != mouseCaptures.rend(); ++el)
                    {
                    (*el)->_mouseMove(event.motion, *this);
                    if((*el)->mouseMove)
                        (*el)->mouseMove(**el, event.motion, *this);
                    }
                //std::cout << event.motion.x << ", " << event.motion.y << std::endl;
                client.mouseMoved(event.motion, *this);
                break;
            case SDL_MOUSEBUTTONDOWN:
                client.mouseDown(event.button, *this);
                break;
            case SDL_MOUSEBUTTONUP:
                for (UIElement* el : mouseCaptures)
                    {
                    el->_mouseUp(event.button, *this);
                    if (el->mouseUp)
                        el->mouseUp(*el, event.button, *this);
                    }
                client.mouseUp(event.button, *this);
                break;
            case SDL_MOUSEWHEEL:
                client.mouseWheel(event.wheel, *this);
                break;
            case SDL_KEYDOWN:
                {
                client.keyDown(event.key, *this);
                break;
                }
            case SDL_WINDOWEVENT:
                switch (event.window.event)
                    {
                    case SDL_WINDOWEVENT_RESIZED: //called after SIZE_CHANGED user or system changes window size.
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED: //always called
                        std::cout << "SDL: window size changed\n";
                        resizeViewportToWindow(window);
                        client.resize(event.window, *this);
                        canvas = NULL;
                        break;
                    }
                break;
            case SDL_QUIT:
                quit = true;
                break;
            default:
                break;
            }
        }
    return eventsHappened;
    }

void SDLSkiaWindow::resizeViewportToWindow(SDL_Window* window)
    {
    SDL_GL_GetDrawableSize(window, &dw, &dh);
    glViewport(0, 0, dw, dh);
    }

