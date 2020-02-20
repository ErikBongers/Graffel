#pragma once
namespace graffel
{

class Timeline;

class Tick
    {
    public:
        Timeline* timeline = 0;
    public:
        virtual bool tick() { return true; }
        void setTimeline(Timeline& timeline) { this->timeline = &timeline; }
    };

class IntervalTick : public Tick
    {
    private:
        long long ticks = 0;
        long long interval = 0; //nanoseconds

    public:
        IntervalTick(long long milliseconds) : interval(milliseconds*1000000) {};
        virtual bool tick() override;
    };
 }
