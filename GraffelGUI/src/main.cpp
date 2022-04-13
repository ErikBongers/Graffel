#include "pch.h"
#include <core/SDLSkiaApp.h>
#include "GraffelWindowClient.h"

#undef main

int main(int argc, char** argv) {
    SDLSkiaApp app;
    auto wndClient = GraffelWindowClient();

    if (app.createWindow("Graffel 0.0", (int)(app.dm.w * 0.7), (int)(app.dm.h * 0.7), wndClient, 0))
        app.startEventLoop();

    return 0;
    }
