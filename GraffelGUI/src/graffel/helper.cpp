#include "helper.h"

using namespace std::chrono_literals;

double getAlpha(T markerTimes[], int marker1, int marker2, T now)
    {
    T t = now - 0s; // 0 = offset for this timeline.
                    //t can be assumed to be between marker 1 and 2
                    //get times for marker1 and marker2
    T t1 = markerTimes[marker1];
    T t2 = markerTimes[marker2];
    return (t-t1)/(t2-t1);
    }

