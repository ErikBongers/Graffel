#include "Generator.h"
#include "Generator.h"
#include <sstream>

std::string Generator::generateTimeline(Timeline* timeline)
    {
    std::ostringstream sstr;
    //resolve timeline.
    std::vector<TimelineMarker> keyFrames;
    int index = 0;
    double time = 0;
    keyFrames.emplace_back(index++, "", time);
    for (auto& frame : timeline->keyframes)
        {
        if(frame->isTimepoint)
            time = frame->value.numberValue;
        else
            time += frame->value.numberValue;
        keyFrames.emplace_back(index++, frame->markerId.stringValue, time);
        }

    sstr << "//GENERATED CODE" << std::endl;
    sstr << "#include <chrono>" << std::endl;
    sstr << R"(#include "D:\Documents\Programming\CppProjects\Graffel\GraffelGUI\src\graffel\helper.h")" << std::endl;
    sstr << "using namespace std::chrono_literals;" << std::endl;
    sstr << std::endl;
    sstr << "T timeline1_markerTimes[" << keyFrames.size() << "] = " << std::endl;
    sstr << "{" << std::endl;
    for (auto& frame : keyFrames)
        {
        sstr << frame.time << "s,";
        if(!frame.markerId.empty())
            sstr << " // #" << frame.markerId;
        sstr << std::endl;
        }
    sstr << "};" << std::endl;
    sstr << std::endl;

    sstr << "int timeline1_lastPassedMarker(T now) //returns the last past marker number" << std::endl;
    sstr << "{" << std::endl;
    sstr << "    T t = now - 0s; // 0 = offset for this timeline." << std::endl;
    for (int i = keyFrames.size()-1; i > 0 ; i--)
        {
        sstr << "    if(t >= "<< keyFrames[i].time << "s)" << std::endl;
        sstr << "        return "<< i << "; // #" << keyFrames[i].markerId << std::endl;
        }
    sstr << "    return 0; //this code works also for times before the timeline even starts." << std::endl;
    sstr << "};" << std::endl;
    return sstr.str();
    }
