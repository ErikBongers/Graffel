#include "pch.h"
#include "TestWindowClient.hpp"

void TestWindowClient::initialize(SDLSkiaWindow& window)
    {
    full.backgroundColor = SK_ColorWHITE;
    full.resize = [](UIElement& e, SDL_WindowEvent& event, SDLSkiaWindow& window) {
        e.rect = SkRect::MakeWH((SkScalar)window.getWidth(), (SkScalar)window.getHeight());
        window.setInvalid();
        };

    toolbar.rect = SkRect::MakeXYWH(20, 10, 500, 40);
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

    square1.rect = SkRect::MakeXYWH(200, 200, 200, 200);
    square1.backgroundColor = SK_ColorBLUE;
    infiniteCanvas += square1;

    bullet.rect = SkRect::MakeXYWH(300, 300, 20, 20);
    bullet.mouseMove = [](UIElement& e, SDL_MouseMotionEvent& event, SDLSkiaWindow& window) {
        Bullet& bullet = (Bullet&)e;
        if (bullet.hitTest((SkScalar)event.x, (SkScalar)event.y))
            {
            bullet.color = SK_ColorYELLOW;
            window.addMouseCapture(bullet); //todo: not efficient.
            }
        else
            {
            bullet.color = SK_ColorRED;
            window.removeMouseCapture(bullet);
            }
        window.setInvalid();//todo: not efficient.
        };

    infiniteCanvas += bullet;

    blueChild.rect = SkRect::MakeXYWH(10, 10, 20, 20);
    blueChild.mouseMove = [](UIElement& e, SDL_MouseMotionEvent& event, SDLSkiaWindow& window) {
        Bullet& bullet = (Bullet&)e;
        if (bullet.hitTest((SkScalar)event.x, (SkScalar)event.y))
            {
            bullet.color = SK_ColorYELLOW;
            window.addMouseCapture(bullet); //todo: not efficient.
            }
        else
            {
            bullet.color = SK_ColorRED;
            window.removeMouseCapture(bullet);
            }
        window.setInvalid();//todo: not efficient.
        };
    square1 += blueChild;

    p1.rect = SkRect::MakeXYWH(80, 80, 10, 10);
    infiniteCanvas += p1;

    p2.rect = SkRect::MakeXYWH(100, 80, 10, 10);
    infiniteCanvas += p2;

    p3.rect = SkRect::MakeXYWH(80, 100, 10, 10);
    infiniteCanvas += p3;

    p4.rect = SkRect::MakeXYWH(100, 100, 10, 10);
    infiniteCanvas += p4;

    curve1.rect = SkRect::MakeXYWH(0,0,1,1); //must be at (0, 0) to work, currently!!!
    curve1.p1 = &p1;
    curve1.p2 = &p2;
    curve1.p3 = &p3;
    curve1.p4 = &p4;
    window.addMouseCapture(curve1);
    curve1.mouseMove = [](UIElement& e, SDL_MouseMotionEvent& event, SDLSkiaWindow& window) {
        Curve& curve = (Curve&)e;
        SkPoint mouse = SkPoint::Make((SkScalar)event.x, (SkScalar)event.y);
        curve.mapPixelsToPoints(&mouse, 1);
        SkColor col = SK_ColorRED;
        if (curve.bounds.contains(mouse.fX, mouse.fY))
            {
            if (curve.outline.contains(mouse.fX, mouse.fY))
                col = SK_ColorYELLOW;
            }
        curve.color = col;
        window.setInvalid();//todo: not efficient.
        };
    infiniteCanvas += curve1;

    imgButton.rect = SkRect::MakeXYWH(400, 300, 26, 26);
    imgButton.xOffset = imgButton.yOffset = 3;
    infiniteCanvas += imgButton;
    window.addMouseCapture(infiniteCanvas); //todo: put in constructor.
    }

void TestWindowClient::update(SDLSkiaWindow& window)
    {
    }

void TestWindowClient::draw(SDLSkiaWindow& window)
    {
    SkCanvas& c = window.Canvas();
    full.drawAll(window);
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
