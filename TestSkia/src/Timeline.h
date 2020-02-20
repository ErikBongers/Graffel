#pragma once

namespace graffel
{

class Tick;
class Block;

class Timeline
    {
    private:
        Timeline();

        Timeline& parent;
        std::list<Timeline*> children;
        Tick* _tick;
        Block* block;

        std::chrono::time_point<std::chrono::steady_clock> startTime;
        std::chrono::nanoseconds ellapsedTime = std::chrono::nanoseconds::zero();

        void init();

    public:
        //////STATE///////

        long long iCounter; 

        //////////////////

    public:
        bool tick();
        Timeline(Timeline& parent);
        static Timeline& createMasterTimeline();
        long long getEllapsedSeconds() { return std::chrono::floor<std::chrono::seconds>(ellapsedTime).count(); }
        std::chrono::nanoseconds getEllapsedTime() { return ellapsedTime; }
        Timeline& createChild();
        void setTick(Tick* tick);
        void setBlock(Block* block) { this->block = block;  }
        Timeline& getRoot();
    };

}//namespace

