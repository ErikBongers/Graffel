#include "pch.h"
#include "GraffelWindowClient.hpp"

void GraffelWindowClient::initialize(SDLSkiaWindow& window)
    {
    masterTimeline = graffel::Timeline::createMasterTimeline();
    //masterTimeline.setTick(new graffel::IntervalTick(1000));
    //graffel::Timeline& halfTime = masterTimeline.createChild();
    //halfTime.setTick(new graffel::IntervalTick(100));
    masterTimeline->setBlock(&b);


    full.backgroundColor = SK_ColorDKGRAY;
    full.rect = SkRect::MakeWH((SkScalar)window.getWidth(), (SkScalar)window.getHeight());
    full.resize = [](UIElement& e, SDL_WindowEvent& event, SDLSkiaWindow& window) {
        e.rect = SkRect::MakeWH((SkScalar)window.getWidth(), (SkScalar)window.getHeight());
        window.setInvalid();
        };

    UIElement& toolbar = *new UIElement();
    toolbar.rect = SkRect::MakeXYWH(10, 10, 500, 40);
    toolbar.backgroundColor = SK_ColorLTGRAY;
    full += toolbar;

    UIElement* button1 = new UIElement();
    button1->rect = SkRect::MakeXYWH(5, 5, 20, 20);
    button1->backgroundColor = SK_ColorDKGRAY;
    toolbar += button1;

    UIElement* button2 = new UIElement();
    button2->rect = SkRect::MakeXYWH(5 + 20 + 5, 5, 20, 20);
    button2->backgroundColor = SK_ColorDKGRAY;
    toolbar += button2;
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
    //if (event.motion.state == SDL_PRESSED) {
//    SkRect& rect = state->fRects.back();
//    rect.fRight = (SkScalar)event.motion.x;
//    rect.fBottom = (SkScalar)event.motion.y;
//    }
    }

void GraffelWindowClient::mouseClicked(SDL_MouseMotionEvent& event, SDLSkiaWindow& window)
    {
    window.setInvalid();
    //if (event.button.state == SDL_PRESSED) {
//    state->fRects.push_back() = SkRect::MakeLTRB(SkIntToScalar(event.button.x),
//                                                 SkIntToScalar(event.button.y),
//                                                 SkIntToScalar(event.button.x),
//                                                 SkIntToScalar(event.button.y));
//    }
    }

void GraffelWindowClient::resize(SDL_WindowEvent& event, SDLSkiaWindow& window)
    {
    full.trickleResizeEvent(event, window);
    }
