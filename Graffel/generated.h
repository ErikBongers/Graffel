//GENERATED CODE
#include <chrono>
#include "D:\Documents\Programming\CppProjects\Graffel\GraffelGUI\src\graffel\helper.h"
using namespace std::chrono_literals;

T timeline1_markerTimes[6] = 
{
0s,
12s, // #markerOne
34s, // #markerTwo
44s, // #markerThree
50s,
70s, // #lastOne
};

int timeline1_lastPassedMarker(T now) //returns the last past marker number
{
    T t = now - 0s; // 0 = offset for this timeline.
    if(t >= 70s)
        return 5; // #lastOne
    if(t >= 50s)
        return 4; // #
    if(t >= 44s)
        return 3; // #markerThree
    if(t >= 34s)
        return 2; // #markerTwo
    if(t >= 12s)
        return 1; // #markerOne
    return 0; //this code works also for times before the timeline even starts.
};
