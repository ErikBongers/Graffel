#include "pch.h"
#include <fstream>
#include "core/SDLSkiaWindow.h"
#include "core/Functions.h"
#include "TestWindowClient.hpp"
#include "json/json.h"
#include "json/Tokenizer.h"
#include "json/Parser.h"
#include "resources/Zip.h"
#include "model/Document.h"

#undef main

int mainGUI(int argc, char** argv) {
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
    TestWindowClient& tst = *(new TestWindowClient());
    SDLSkiaWindow* wnd = new SDLSkiaWindow(tst);

    if (wnd->createWindow((int)(dm.w * 0.7), (int)(dm.h * 0.7), kStencilBits, kMsaaSampleCount))
        wnd->startEventLoop();
    wnd->destroyWindow();

    //Quit SDL subsystems
    SDL_Quit();
    return 0;
}

using namespace json;
using namespace idea;
void testJson()
    {
    Document doc;

    auto node = doc.createNode();
    node->title = "den title";
    node->dscr = "omschrijving";
    node->body = "sexy body...";

    node = doc.createNode();
    node->title = "den title 2";
    node->dscr = "omschrijving 2";
    node->body = "sexy body... 2";

    std::string fileName = R"(C:\Users\erikb\Desktop\test.idea)";
    std::ofstream ofs(fileName);
    ofs << doc;
    ofs.close();
    
    std::cout << doc;

    std::ifstream ifs(fileName);
    json::Tokenizer tok(ifs);
    json::Parser parser(tok);

    Object object = parser.parse();

    ifs.close();

    Document doc2(object);
    auto node2 = doc2.createNode();

    std::cout << "-------------" << std::endl << doc2;
    }

int main(int argc, char** argv) 
    {
    //mainGUI(argc, argv);
    testJson();
    }

