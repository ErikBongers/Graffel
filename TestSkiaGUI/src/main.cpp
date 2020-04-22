#include "pch.h"
#include "core/SDLSkiaWindow.h"
#include "core/Functions.h"
#include "TestWindowClient.hpp"
#include "resources/Zip.h"

#undef main
int main(int argc, char** argv) {
    Zip zip(R"(D:\Documents\Programming\CppProjects\Graffel\TestSkiaGUI\src\images\images.zip)");
    //FileBuffer b = zip.getFile("ColoredSquare.png");
    FileBuffer b = zip.getFile(1);
    zip.close();














    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // For all other clients we use the core profile and operate in a window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    static const int kStencilBits = 8;  // Skia needs 8 stencil bits
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, kStencilBits);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // If you want multisampling, uncomment the below lines and set a sample count
    static const int kMsaaSampleCount = 0; //4;
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, kMsaaSampleCount);

    /*
     * In a real application you might want to initialize more subsystems
     */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        handle_error();
        return 1;
    }

    // Setup window
    // This code will create a window with the same resolution as the user's desktop.
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        handle_error();
        return 1;
    }
    SDL_StartTextInput();

    TestWindowClient* client = new TestWindowClient();
    SDLSkiaWindow* wnd = new SDLSkiaWindow(*client);
    
    if(wnd->createWindow((int)(dm.w*0.7), (int)(dm.h*0.7), kStencilBits, kMsaaSampleCount))
        wnd->startEventLoop();
    wnd->destroyWindow();

    //Quit SDL subsystems
    SDL_Quit();
    return 0;
}
