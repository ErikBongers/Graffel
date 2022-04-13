#pragma once
#include <chrono>

using T =  std::chrono::duration<double>;

double getAlpha(T markerTimes[], int marker1, int marker2, T now);
