#pragma once
#include <vector>


class Variant
    {
    struct VMarker
        {
        double value;
        int attachedMarker;
        bool fadeIn;
        bool fadeOut;
        VMarker(double value, int attachedMarker, bool fadeIn, bool fadeOut);
        };

    std::vector<VMarker> vmarkers;
    public:
        void addMarker(double value, int attachedMarker, bool fadeIn, bool fadeOut);
        double interpolate(int marker1, double t);
    };

