#include "pch.h"
#include "GraffelWindowClient.hpp"

void GraffelWindowClient::initialize(SDLSkiaWindow& window)
    {
    masterTimeline = graffel::Timeline::createMasterTimeline();
    //masterTimeline.setTick(new graffel::IntervalTick(1000));
    //graffel::Timeline& halfTime = masterTimeline.createChild();
    //halfTime.setTick(new graffel::IntervalTick(100));
    masterTimeline->setBlock(&b);

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

    }

void GraffelWindowClient::update(SDLSkiaWindow& window)
    {
    //if (masterTimeline->tick())
    //    window.setInvalid();
    }

void GraffelWindowClient::draw(SDLSkiaWindow& window)
    {
    SkCanvas& c = window.Canvas();
    //SkRandom rand;
    b.draw(*masterTimeline, c);
    SkPaint paint;
    paint.setColor(SK_ColorLTGRAY);
    paint.setStyle(SkPaint::Style::kFill_Style);
    SkRect rect = SkRect::MakeXYWH(300, 100, 100, 100);
    c.drawRect(rect, paint);
    full.drawAll(0, 0, window);
    }

void GraffelWindowClient::mouseMoved(SDL_MouseMotionEvent& event, SDLSkiaWindow& window)
    {
    if(button1.hitTest(event.x, event.y))
        button1.backgroundColor = SK_ColorYELLOW;
    else
        button1.backgroundColor = SK_ColorDKGRAY;
    window.setInvalid();//TODO: don't invalidate on every mouseMove
    infiniteCanvas.mouseMoved(event, window);
    }

void GraffelWindowClient::mouseDown(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    window.setInvalid();
    infiniteCanvas.mouseDown(event, window);
    }

void GraffelWindowClient::mouseUp(SDL_MouseButtonEvent& event, SDLSkiaWindow& window)
    {
    infiniteCanvas.mouseUp(event, window);
    }

void GraffelWindowClient::mouseWheel(SDL_MouseWheelEvent_EX& event, SDLSkiaWindow& window)
    {
    infiniteCanvas.mouseWheel(event, window);
    }

void GraffelWindowClient::resize(SDLSkiaWindow& window)
    {
    full.trickleResizeEvent(window);
    }
