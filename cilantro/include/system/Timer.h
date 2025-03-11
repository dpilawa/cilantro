#ifndef _TIMER_H_
#define _TIMER_H_

#include "cilantroengine.h"
#include <chrono>

namespace cilantro {

class __CEAPI Timer
{
public:
    __EAPI Timer ();
    __EAPI virtual ~Timer ();

    // return delta-time of currently rendered frame (in seconds)
    __EAPI float GetFrameDeltaTime ();
    // return rendering time of last frame (in seconds)
    __EAPI float GetFrameRenderTime ();
    // reset split time
    __EAPI void ResetSplitTime ();
    // return time since split time (in seconds)
    __EAPI float GetTimeSinceSplitTime ();
    // calculate delta-time and reset begin timestamps on new frame
    void Tick ();
    // set end timestamp and calculate frame rendering time
    void Tock ();

private:
    // delta-time of current frame
    std::chrono::duration<float> frameDeltaTime;
    // rendering time of current frame
    std::chrono::duration<float> frameRenderTime;
    // start time of current frame (set on Tick)
    std::chrono::high_resolution_clock::time_point timeOfFrameStart;
    // end time of current frame (set on Tock)
    std::chrono::high_resolution_clock::time_point timeOfFrameEnd;
    // saved split time
    std::chrono::high_resolution_clock::time_point splitTime;
};

} // namespace cilantro

#endif
