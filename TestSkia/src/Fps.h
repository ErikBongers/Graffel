#pragma once

namespace graffel
{
class Fps
    {
    protected:
        unsigned int m_fps;
        float msPerFrame = 0.0f;
        unsigned int frameCount;
        std::chrono::time_point<std::chrono::steady_clock> startTime;
        std::chrono::time_point<std::chrono::steady_clock> frameStart;
        std::chrono::nanoseconds totalFrameTime;

        const long long RESOLUTION = 1000000000; // 1 second in nanoseconds
        const long long ONE_SECOND = 1000000000;
        const long long ONE_MILLISEC = 1000000;
    public:
        unsigned int getFps() const { return m_fps; }
        float getMsPerFrame() const { return msPerFrame; }

    public:
        // Constructor
        Fps() : m_fps(0), frameCount(0)
            {
            startTime = std::chrono::steady_clock::now();
            }

        // Update
        void beginFrame()
            {
            frameStart = std::chrono::steady_clock::now();
            frameCount++;
            }

        void endFrame()
            {
            auto ellapsedTime = std::chrono::steady_clock::now() - startTime;
            auto frameTime = std::chrono::steady_clock::now() - frameStart;
            totalFrameTime += frameTime;
            
            if (ellapsedTime.count() > RESOLUTION)
                {
                m_fps = frameCount/(ellapsedTime.count() /ONE_SECOND);
                //msPerFrame = (ellapsedTime.count() / ONE_MILLISEC) / frameCount;
                msPerFrame = (totalFrameTime.count() / ONE_MILLISEC) / frameCount;
                // reset each interval (resolution).
                frameCount = 0;
                using namespace std::chrono_literals;
                totalFrameTime = 0ns;
                startTime = std::chrono::steady_clock::now();
                }
            }

    };
}