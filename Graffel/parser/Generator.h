#pragma once
#include "Parser.h"
#include <string>

struct TimelineMarker
    {
    int index;
    std::string markerId;
    double time; //TODO: replace with (relative) timepoint.
    TimelineMarker(int index, const std::string& markerId, double time)
        : index(index), markerId(markerId), time(time) {}
    };

class Generator
    {
    public:
        std::string generateTimeline(Timeline* timeline);
    };