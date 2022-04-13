#include "core/SDLSkiaWindow.h"
#include "core/UIElement.h"
#include "core/SplittableArea.h"
#include "controls/InfiniteCanvas.h"
#include <graffel/Timeline.h>
#include <graffel/Block.h>


class GraffelWindowClient : public WindowClient
    {
    private:
        graffel::Block* block;
        graffel::Timeline* masterTimeline;


        SplittableArea mainView;
        UIElement full;
        UIElement toolbar;
        UIElement button1;
        UIElement button2;
        InfiniteCanvas infiniteCanvas;
        UIElement square1;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::duration<double> now;

    public:
        void initialize() override;
        void onIdle() override;
        SplittableArea* getMainView() override { return &mainView; }
    };