#include "pch.h"
#include "TestWindowClient.hpp"
#include "resources/Resources.h"
#include "texteditor/editor.h"

void TestWindowClient::initialize(SDLSkiaWindow& window)
    {
    full.backgroundColor = SkColorSetRGB(50,50,50);
    full.resize = [](UIElement& e, SDL_WindowEvent& event, SDLSkiaWindow& window) {
        e.rect = SkRect::MakeWH((SkScalar)window.getWidth(), (SkScalar)window.getHeight());
        window.setInvalid();
        };

    toolbar.resize = [](UIElement& e, SDL_WindowEvent& event, SDLSkiaWindow& window) {
        e.rect = SkRect::MakeLTRB(0, 0, (SkScalar)window.getWidth(), 40);
        window.setInvalid();
        };
    full += toolbar;
    Resources res(R"(D:\Documents\Programming\CppProjects\Graffel\TestSkiaGUI\src\images\images.zip)", R"(D:\Documents\Programming\CppProjects\Graffel\TestSkiaGUI\src\images)");

    imgButton.rect = SkRect::MakeXYWH(5,5,20,23);
    imgButton.img = res.get("SquareSquare.png");
    imgButton.xOffset = imgButton.yOffset = 1;
    imgButton.mouseUp = [this](UIElement& e, SDL_MouseButtonEvent& event, SDLSkiaWindow& window) {
        std::cout << "Clicked!" << std::endl;
        window.setInvalid();
        };
    toolbar += imgButton;

    imgButton2.rect = SkRect::MakeXYWH(5 + 20 + 5, 5, 20, 23);
    imgButton2.img = res.get("ColoredSquare.png");
    imgButton2.xOffset = imgButton2.yOffset = 1;
    imgButton2.mouseUp = [this](UIElement& e, SDL_MouseButtonEvent& event, SDLSkiaWindow& window) {
        std::cout << "Clicked 2!" << std::endl;
        window.setInvalid();
        };
    toolbar += imgButton2;

    imgButton3.rect = SkRect::MakeXYWH(5 + 20 + 5 + 20 + 5, 5, 20, 23);
    imgButton3.img = res.get("RedSquare.png");
    imgButton3.xOffset = imgButton3.yOffset = 1;
    imgButton3.mouseUp = [this](UIElement& e, SDL_MouseButtonEvent& event, SDLSkiaWindow& window) {
        std::cout << "Clicked 3!" << std::endl;
        window.setInvalid();
        };
    toolbar += imgButton3;

    infiniteCanvas.resize = [](UIElement& e, SDL_WindowEvent& event, SDLSkiaWindow& window) {
        e.rect = SkRect::MakeLTRB(10, 30, (SkScalar)window.getWidth() - 10, (SkScalar)window.getHeight() - 10);
        window.setInvalid();
        };
    infiniteCanvas.backgroundColor = SkColorSetRGB(35,35,35);
    full += infiniteCanvas;

    square1.rect = SkRect::MakeXYWH(200, 200, 100, 100);
    square1.backgroundColor = SkColorSetARGB(40, 50,50,255);
    infiniteCanvas += square1;

    bullet.rect = SkRect::MakeXYWH(400, 300, 20, 40);
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

    editor1.setFont(SkFont(SkTypeface::MakeFromName("sans-serif", //serif, monospace,...
            SkFontStyle(SkFontStyle::kNormal_Weight, SkFontStyle::kNormal_Width, SkFontStyle::kUpright_Slant)), 18));
    const char* txt = "Hellow, Earl D!";
    editor1.insert(SkPlainTextEditor::Editor::TextPosition{ 0, 0 }, txt, strlen(txt));
    editor1.rect = SkRect::MakeXYWH(300, 300, 200, 200);
    infiniteCanvas += editor1;


    window.addMouseCapture(infiniteCanvas); //todo: put in constructor.
    }

void TestWindowClient::onIdle(SDLSkiaWindow& window)
    {
    full.trickleIdle(window);
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
