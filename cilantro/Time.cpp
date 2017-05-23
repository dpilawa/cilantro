#include "Time.h"

// duration of current frame
std::chrono::duration<double> Time::frameDeltaTime;
// start time of current frame (set on Tick)
std::chrono::steady_clock::time_point Time::timeOfFrameStart;

Time::Time ()
{
}

Time::~Time ()
{
}

// get duration of currently rendered frame (in seconds)
double Time::GetFrameDeltaTime ()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(frameDeltaTime).count () / 1000000.0f;
}

// reset tracked frame durations on new frame
void Time::Tick () 
{
	auto timeOfPreviousFrameStart = timeOfFrameStart;
	timeOfFrameStart = std::chrono::steady_clock::now ();
	frameDeltaTime = timeOfFrameStart - timeOfPreviousFrameStart;
}