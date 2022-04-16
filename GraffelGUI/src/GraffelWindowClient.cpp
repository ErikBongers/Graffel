#include "pch.h"
#include "GraffelWindowClient.h"
#include "graffel/Block.h"
#include "graffel/Variant.h"
#include "graffel/helper.h"
/*
TEST CASE
------------
[
@10s #startMove //#1 @10s
5s #nothing //#2 @15s
15s #endMove  //#3 @30s
]
{ //main block: keep background clear.

    { //test block. set a bkg
    .background = YELLOW.
    .x = 0_ #startMove //timeline1.#1
        _100 #endMove //timeline1.#3
    }
}
*/

//GENERATED CODE
using namespace std::chrono_literals;

int timeline1_lastPassedMarker(T now) //returns the last past marker number
    {
    T t = now - 0s; // 0 = offset for this timeline.
    if(t >= 30s)
        return 3; // #endMove
    if(t >= 15s)
        return 2; // #nothing
    if (t >= 10s)
        return 1; // #startMove
    return 0; //this code works also for times before the timeline even starts.
    };

T timeline1_markerTimes[4] = 
    {
    0s, //is this ever consulted?
    10s, // @10s #startMove
    15s, // 5s #nothing
    30s, // 15s #endMove
    };

class Block1 : public graffel::Block
    {
    private:
        Variant varX;
    public:
        Block1(graffel::Timeline &timeline) : Block(timeline) 
            {
            varX.addMarker(0, 0, false, true);
            varX.addMarker(100, 1, true, false);
            }
        void drawBlock() override
            {
            std::string strCounter = std::to_string(Block::timeline.getEllapsedSeconds());
            SkPaint paint;
            paint.setColor(SK_ColorBLACK); // move to init function?
            SkFont font;
            Canvas().drawString(strCounter.c_str(), 100.0f, 130.0f, font, paint);
            //Graffel would generate drawing code here, instead of the above stuff.
            }
        void update(T now) override
            {
            double x = 0;
            // resolving .x = ...
            if(timeline1_lastPassedMarker(now) >= 3)// > #endMove
               x = 100; //varX.marker[1].value
            else if(timeline1_lastPassedMarker(now) >= 1) // > #startMove : between markers #startMove and #endMove
                {
                auto alpha = getAlpha(timeline1_markerTimes, 1/*#startMove*/, 3/*#endMove*/, now);
                x = varX.interpolate(0, alpha); //#startMove = varX.marker[0]
                }
            else
                x = 0; //varX.marker[0].value
            auto w = rect.width();
            rect.fLeft = x;
            rect.fRight = x+w;
            }
    };

//END GENERATED CODE


void GraffelWindowClient::initialize()
    {

    masterTimeline = graffel::Timeline::createMasterTimeline();
    //masterTimeline.setTick(new graffel::IntervalTick(1000));
/*
    //graffel::Timeline& halfTime = masterTimeline.createChild();
    //halfTime.setTick(new graffel::IntervalTick(100));
    masterTimeline->setBlock(&b);
*/

    full.backgroundColor = SK_ColorWHITE;
    mainView.setContent(&full);
/*
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
    */
    block = new Block1(*masterTimeline);
    block->rect = SkRect::MakeXYWH(0, 100, 500, 250);
    block->backgroundColor = SK_ColorYELLOW;
    full += block; 
    mainView.getWindow()->drawFps({10, 10}, SK_ColorBLUE);
    startTime = std::chrono::steady_clock::now();
    }

void GraffelWindowClient::onIdle()
    {
    if (masterTimeline->tick())
        mainView.getWindow()->setInvalid();
    now = std::chrono::steady_clock::now() - startTime;
    block->update(now);
    }
