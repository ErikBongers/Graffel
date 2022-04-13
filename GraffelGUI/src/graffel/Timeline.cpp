#include <list>
#include <chrono>
#include "Timeline.h"
#include "Tick.h"

graffel::Timeline::Timeline(Timeline& parent) : parent(parent)
    {
    }

graffel::Timeline* graffel::Timeline::createMasterTimeline() { 
    auto newT = new Timeline();
    newT->init();
    return newT; //TODO: memoryleak !
    }

graffel::Timeline& graffel::Timeline::createChild()
    {
    auto child = new Timeline(*this);
    children.push_back(child);
    return *child;
    }

void graffel::Timeline::setTick(Tick* tick) 
    { 
    _tick = tick; 
    _tick->setTimeline(*this); 
    }

graffel::Timeline::Timeline() : parent(*this)
    {
    init();
    }


void graffel::Timeline::init()
    {
    this->_tick = new Tick();
    startTime = std::chrono::steady_clock::now();
    }

/**
* returns true if the tick triggered a marker (named or fixed interval)
*/
bool graffel::Timeline::tick()
    {
    ellapsedTime = std::chrono::steady_clock::now() - startTime;

    bool theTick = false;
    theTick |= _tick->tick();
    for (auto const& tl : children)
        {
        theTick |= tl->tick();
        }

    return theTick;
    }

graffel::Timeline& graffel::Timeline::getRoot()
    {
    if (this == &parent)
        return *this;
    else
        return parent.getRoot();
    }
