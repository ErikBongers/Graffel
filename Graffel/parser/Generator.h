#pragma once
#include "Parser.h"
#include <string>

struct TimelineMarker
    {
    int index;
    std::string marker;
    double time; //TODO: replace with (relative) timepoint.
    TimelineMarker(int index, const std::string& marker, double time)
        : index(index), marker(marker), time(time) {}
    };

class Generator
    {
    public:
        std::string generateTimeline(Timeline* timeline);
    };