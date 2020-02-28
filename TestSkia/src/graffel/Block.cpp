#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include <list>
#include <chrono>
#include <string>
#include "Block.h"
#include "Timeline.h"

void graffel::Block::draw(Timeline& timeline, SkCanvas& canvas)
    {
    std::string strCounter = std::to_string(timeline.getEllapsedSeconds());
    paint.setColor(SK_ColorBLACK); // move to init function?
    canvas.drawString(strCounter.c_str(), 100.0f, 130.0f, font, paint);
    }

graffel::Block::Block()
    {
    }
