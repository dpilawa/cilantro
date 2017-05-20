#include "Time.h"

// duration of current frame
std::chrono::duration<float> Time::frameDeltaTime;
// start time of current frame (set on Tick)
std::chrono::high_resolution_clock::time_point Time::timeOfFrameStart;

Time::Time ()
{
}

Time::~Time ()
{
}

// get duration of currently rendered frame (in seconds)
float Time::GetFrameDeltaTime ()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(frameDeltaTime).count () / 1000000.0f;
}

// reset tracked frame durations on new frame
void Time::Tick () 
{
	auto timeOfPreviousFrameStart = timeOfFrameStart;
	timeOfFrameStart = std::chrono::high_resolution_clock::now ();
	frameDeltaTime = timeOfFrameStart - timeOfPreviousFrameStart;
}