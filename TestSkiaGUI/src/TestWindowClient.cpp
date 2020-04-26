#include "pch.h"
#include "TestWindowClient.hpp"
#include "resources/Resources.h"
#include "texteditor/editor.h"

void TestWindowClient::initialize()
    {
    full.backgroundColor = SkColorSetRGB(50,50,50);
    full.resize = [](UIElement& e, SDL_WindowEvent& event) {
        e.rect = SkRect::MakeWH((SkScalar)e.getWindow()->getWidth(), (SkScalar)e.getWindow()->getHeight());
        e.getWindow()->setInvalid();
        };

    toolbar.resize = [](UIElement& e, SDL_WindowEvent& event) {
        e.rect = SkRect::MakeLTRB(0, 0, (SkScalar)e.getWindow()->getWidth(), 40);
        e.getWindow()->setInvalid();
        };
    full += toolbar;
    Resources res(R"(D:\Documents\Programming\CppProjects\Graffel\TestSkiaGUI\src\images\images.zip)", R"(D:\Documents\Programming\CppProjects\Graffel\TestSkiaGUI\src\images)");

    imgButton.rect = SkRect::MakeXYWH(5,5,20,23);
    imgButton.img = res.get("SquareSquare.png");
    imgButton.xOffset = imgButton.yOffset = 1;
    imgButton.mouseUp = [this](UIElement& e, SDL_MouseButtonEvent& event) {
        std::cout << "Clicked!" << std::endl;
        e.getWindow()->setInvalid();
        };
    toolbar += imgButton;

    imgButton2.rect = SkRect::MakeXYWH(5 + 20 + 5, 5, 20, 23);
    imgButton2.img = res.get("ColoredSquare.png");
    imgButton2.xOffset = imgButton2.yOffset = 1;
    imgButton2.mouseUp = [this](UIElement& e, SDL_MouseButtonEvent& event) {
        std::cout << "Clicked 2!" << std::endl;
        e.getWindow()->setInvalid();
        };
    toolbar += imgButton2;

    imgButton3.rect = SkRect::MakeXYWH(5 + 20 + 5 + 20 + 5, 5, 20, 23);
    imgButton3.img = res.get("RedSquare.png");
    imgButton3.xOffset = imgButton3.yOffset = 1;
    imgButton3.mouseUp = [this](UIElement& e, SDL_MouseButtonEvent& event) {
        std::cout << "Clicked 3!" << std::endl;
        e.getWindow()->setInvalid();
        };
    toolbar += imgButton3;

    infiniteCanvas.resize = [](UIElement& e, SDL_WindowEvent& event) {
        e.rect = SkRect::MakeLTRB(10, 30, (SkScalar)e.getWindow()->getWidth() - 10, (SkScalar)e.getWindow()->getHeight() - 10);
        e.getWindow()->setInvalid();
        };
    infiniteCanvas.backgroundColor = SkColorSetRGB(35,35,35);
    full += infiniteCanvas;

    square1.rect = SkRect::MakeXYWH(200, 200, 100, 100);
    square1.backgroundColor = SkColorSetARGB(40, 50,50,255);
    infiniteCanvas += square1;

    bullet.rect = SkRect::MakeXYWH(400, 300, 20, 40);
    bullet.mouseMove = [](UIElement& e, SDL_MouseMotionEvent& event) {
        Bullet& bullet = (Bullet&)e;
        if (bullet.hitTest((SkScalar)event.x, (SkScalar)event.y))
            {
            bullet.color = SK_ColorYELLOW;
            e.getWindow()->addMouseCapture(bullet); //todo: not efficient.
            }
        else
            {
            bullet.color = SK_ColorRED;
            e.getWindow()->removeMouseCapture(bullet);
            }
        e.getWindow()->setInvalid();//todo: not efficient.
        };

    infiniteCanvas += bullet;

    blueChild.rect = SkRect::MakeXYWH(10, 10, 20, 20);
    blueChild.mouseMove = [](UIElement& e, SDL_MouseMotionEvent& event) {
        Bullet& bullet = (Bullet&)e;
        if (bullet.hitTest((SkScalar)event.x, (SkScalar)event.y))
            {
            bullet.color = SK_ColorYELLOW;
            e.getWindow()->addMouseCapture(bullet); //todo: not efficient.
            }
        else
            {
            bullet.color = SK_ColorRED;
            e.getWindow()->removeMouseCapture(bullet);
            }
        e.getWindow()->setInvalid();//todo: not efficient.
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
    getWindow()->addMouseCapture(curve1);
    curve1.mouseMove = [](UIElement& e, SDL_MouseMotionEvent& event) {
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
        e.getWindow()->setInvalid();//todo: not efficient.
        };
    infiniteCanvas += curve1;

    editor1.setFont(SkFont(SkTypeface::MakeFromName("sans-serif", //serif, monospace,...
            SkFontStyle(SkFontStyle::kNormal_Weight, SkFontStyle::kNormal_Width, SkFontStyle::kUpright_Slant)), 18));
    
    //TESTS
    
    //const char* txt = "Hellow, Earl Duh! This is the second time that I came across an issue with framing everything. \nI'm sure you know what I'm talking about. It probably has to do with margins, \nbut I think we need to double check. I'll follow up on the second page with some more info.";
    const char* txt = "aaa\nbbb";
    editor1.insert(txt);
    editor1.setEditMode(true);
    auto str = editor1.txt.doc.selectionToString();
    std::ofstream ofs(R"(D:\Documents\Programming\CppProjects\Graffel\TestSkiaGUI\data\dump.txt)");
    ofs << str;
    ofs.close();


    // END TESTS
    editor1.rect = SkRect::MakeXYWH(300, 300, 200, 200);
    editor1.backgroundColor = SkColorSetARGB(128, 30, 30, 255);
    infiniteCanvas += editor1;


    getWindow()->addMouseCapture(infiniteCanvas); //todo: put in constructor.
    }

void TestWindowClient::onIdle()
    {
    full.trickleIdle();
    }

void TestWindowClient::draw()
    {
    SkCanvas& c = getWindow()->Canvas();
    full.drawAll();
    }

void TestWindowClient::mouseMoved(SDL_MouseMotionEvent& event)
    {
    full.trickleMouseMoveEvent(event);
    }

void TestWindowClient::mouseDown(SDL_MouseButtonEvent& event)
    {
    full.trickleMouseDownEvent(event);
    }

void TestWindowClient::mouseUp(SDL_MouseButtonEvent& event)
    {
    full.trickleMouseUpEvent(event);
    }

void TestWindowClient::mouseWheel(SDL_MouseWheelEvent& event)
    {
    infiniteCanvas.mouseWheel(event);
    }

void TestWindowClient::resize(SDL_WindowEvent& event)
    {
    full.trickleResizeEvent(event);
    }

void TestWindowClient::textInput(SDL_TextInputEvent& event)
    {
    full.trickleTextEvent(event);
    }

void TestWindowClient::keyDown(SDL_KeyboardEvent& event)
    {
    full.trickleKeyDown(event);
    }
