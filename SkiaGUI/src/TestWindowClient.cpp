#include "pch.h"
#include "TestWindowClient.hpp"

void TestWindowClient::initialize(SDLSkiaWindow& window)
    {
    full.backgroundColor = SK_ColorWHITE;
    full.resize = [](UIElement& e, SDL_WindowEvent& event, SDLSkiaWindow& window) {
        e.rect = SkRect::MakeWH((SkScalar)window.getWidth(), (SkScalar)window.getHeight());
        window.setInvalid();
        };

    toolbar.rect = SkRect::MakeXYWH(10, 10, 500, 40);
    toolbar.backgroundColor = SK_ColorLTGRAY;
    full += toolbar;

    button1.rect = SkRect::MakeXYWH(5, 5, 20, 20);
    button1.backgroundColor = SK_ColorDKGRAY;
    button1.highlightColor = SK_ColorYELLOW;
    button1.mouseUp = [this](UIElement& e, SDL_MouseButtonEvent& event, SDLSkiaWindow& window) {
        std::cout << "Clicked!" << std::endl;
        this->button2.backgroundColor = SK_ColorRED;
        window.setInvalid();
        };
    toolbar += button1;

    button2.rect = SkRect::MakeXYWH(5 + 20 + 5, 5, 20, 20);
    button2.backgroundColor = SK_ColorDKGRAY;
    toolbar += button2; 

    infiniteCanvas.resize = [](UIElement& e, SDL_WindowEvent& event, SDLSkiaWindow& window) {
        e.rect = SkRect::MakeLTRB(20, 50, (SkScalar)window.getWidth() - 30, (SkScalar)window.getHeight() - 30);
        window.setInvalid();
        };
    infiniteCanvas.backgroundColor = SK_ColorDKGRAY;
    full += infiniteCanvas;

    square1.rect = SkRect::MakeXYWH(200, 200, 20, 20);
    square1.backgroundColor = SK_ColorBLUE;
    infiniteCanvas += square1;

    bullet.rect = SkRect::MakeXYWH(300, 300, 10, 10);
    bullet.backgroundColor = SK_ColorYELLOW;
    infiniteCanvas += bullet;
    }

void TestWindowClient::update(SDLSkiaWindow& window)
    {
    }

void TestWindowClient::draw(SDLSkiaWindow& window)
    {
    SkCanvas& c = window.Canvas();
    SkPaint paint;
    paint.setColor(SK_ColorLTGRAY);
    paint.setStyle(SkPaint::Style::kFill_Style);
    SkRect rect = SkRect::MakeXYWH(300, 100, 100, 100);
    c.drawRect(rect, paint);
    full.drawAll(0, 0, window);
    }

void TestWindowClient::mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window)
    {
    full.trickleMouseMoveEvent(event, window);
    }

void TestWindowClient::mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    window.setInvalid();
    infiniteCanvas.mouseDown(event, window);
    }

void TestWindowClient::mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    full.trickleMouseUpEvent(event, window);
    }

void TestWindowClient::mouseWheel(SDL_MouseWheelEvent& event, SDLSkiaWindow& window)
    {
    infiniteCanvas.mouseWheel(event, window);
    }

void TestWindowClient::resize(SDL_WindowEvent& event, SDLSkiaWindow& window)
    {
    full.trickleResizeEvent(event, window);
    }
