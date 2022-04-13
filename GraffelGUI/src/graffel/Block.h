#pragma once
#include "core/UIElement.h"
#include "Timeline.h"
namespace graffel
{

class Block : public UIElement
    {
    public:
        Timeline& timeline;
        Block(Timeline& timeline) : timeline(timeline) {}
        virtual void drawMe() override;
        virtual void drawBlock() = 0;
        virtual void update(std::chrono::duration<double> now) = 0;
    };

}

