#include "SDLWindow.h"
#include "Functions.h"

void SDLWindow::startEventLoop()
    {
    int loopCounter = 0;

    while (!quit) { // Our application loop
        loopCounter++;

        handleEvents();

        if (masterTimeline->tick())
            {
            fps.beginFrame();

            if (canvas == NULL)
                canvas = createSurfaceAndCanvas(interfac, windowFormat, contextType, grContext);
            SkRandom rand;
            canvas->clear(SK_ColorWHITE);

            b.draw(*masterTimeline, *canvas);

            std::string strFps = std::to_string(fps.getFps());
            std::string strMs = std::to_string((int)fps.getMsPerFrame()) + "ms";
            std::string strLoopCnt = std::to_string(loopCounter) + "loop iterations";
            paint.setColor(SK_ColorBLACK); // move to init function?
            canvas->drawString(strFps.c_str(), 100.0f, 160.0f, font, paint);
            canvas->drawString(strMs.c_str(), 100.0f, 180.0f, font, paint);
            canvas->drawString(strLoopCnt.c_str(), 100.0f, 200.0f, font, paint);

            fps.endFrame();

            canvas->flush();
            SDL_GL_SwapWindow(window);
            }

        }
    }

bool  SDLWindow::createWindow(int width, int height, int stencilBits, int msaaSampleCount)
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


    masterTimeline = graffel::Timeline::createMasterTimeline();
    //masterTimeline.setTick(new graffel::IntervalTick(1000));
    //graffel::Timeline& halfTime = masterTimeline.createChild();
    //halfTime.setTick(new graffel::IntervalTick(100));
    masterTimeline->setBlock(&b);

    }

void SDLWindow::destroyWindow()
    {
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
        }

    SDL_DestroyWindow(window);
    }


SkCanvas* SDLWindow::createSurfaceAndCanvas(sk_sp<const GrGLInterface> interfac, uint32_t windowFormat, int contextType, sk_sp<GrContext> grContext)
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

void SDLWindow::handleEvents()
    {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                //if (event.motion.state == SDL_PRESSED) {
                //    SkRect& rect = state->fRects.back();
                //    rect.fRight = (SkScalar)event.motion.x;
                //    rect.fBottom = (SkScalar)event.motion.y;
                //    }
                break;
            case SDL_MOUSEBUTTONDOWN:
                //if (event.button.state == SDL_PRESSED) {
                //    state->fRects.push_back() = SkRect::MakeLTRB(SkIntToScalar(event.button.x),
                //                                                 SkIntToScalar(event.button.y),
                //                                                 SkIntToScalar(event.button.x),
                //                                                 SkIntToScalar(event.button.y));
                //    }
                break;
            case SDL_KEYDOWN:
                {
                SDL_Keycode key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE) 
                    quit = true;
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
    }

void SDLWindow::resizeViewportToWindow(SDL_Window* window)
    {
    SDL_GL_GetDrawableSize(window, &dw, &dh);
    glViewport(0, 0, dw, dh);
    }
