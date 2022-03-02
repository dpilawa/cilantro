#include "cilantroengine.h"
#include "system/Timer.h"

Timer::Timer ()
{
    ResetSplitTime ();
}

Timer::~Timer ()
{
}

float Timer::GetFrameDeltaTime ()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(frameDeltaTime).count () / 1000000.0f;
}

float Timer::GetFrameRenderTime ()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(frameRenderTime).count () / 1000000.0f;
}

void Timer::ResetSplitTime ()
{
    splitTime = std::chrono::high_resolution_clock::now ();
}

float Timer::GetTimeSinceSplitTime ()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now () - splitTime).count () / 1000000.0f;
}

void Timer::Tick () 
{
    auto timeOfPreviousFrameStart = timeOfFrameStart;
    timeOfFrameStart = std::chrono::high_resolution_clock::now ();
    frameDeltaTime = timeOfFrameStart - timeOfPreviousFrameStart;
}

void Timer::Tock ()
{
    timeOfFrameEnd = std::chrono::high_resolution_clock::now ();
    frameRenderTime = timeOfFrameEnd - timeOfFrameStart;
}
