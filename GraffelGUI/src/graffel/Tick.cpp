#include <chrono>
#include <list>
#include "Tick.h"
#include "Timeline.h"

bool graffel::IntervalTick::tick()
    {
    auto newTicks = this->timeline->getRoot().getEllapsedTime().count()/interval;
    if (newTicks > ticks)
        {
        ticks = newTicks;
        return true;
        }
    return false;
    }

