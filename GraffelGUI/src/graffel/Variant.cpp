#include "pch.h"
#include "Variant.h"

void Variant::addMarker(double value, int attachedMarker, bool fadeIn, bool fadeOut)
    {
    vmarkers.emplace_back(value, attachedMarker, fadeIn, fadeOut);
    }

double Variant::interpolate(int marker1, double t)
    {
    //plain lerp
    double v0 = vmarkers[marker1].value;
    double v1 = vmarkers[marker1+1].value;
    return (1 - t) * v0 + t * v1;
    }

Variant::VMarker::VMarker(double value, int attachedMarker, bool fadeIn, bool fadeOut)
    : value(value), attachedMarker(attachedMarker), fadeIn(fadeIn), fadeOut(fadeOut)
    {
    }
