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

    sstr << "T timeline1_markerTimes[" << keyFrames.size() << "] = " << std::endl;
    sstr << "{" << std::endl;
    for (auto& frame : keyFrames)
        {
        sstr << frame.time << "s,";
        if(!frame.marker.empty())
            sstr << " // #" << frame.marker;
        sstr << std::endl;
        }
    sstr << "};" << std::endl;
    return sstr.str();
    }
