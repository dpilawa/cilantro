#include "Time.h"

// duration of current frame
std::chrono::duration<float> Time::frameDuration;
// time elapsed since start of current frame
std::chrono::duration<float> Time::timeSinceFrameStart;
// start time of current frame (set on Tick)
std::chrono::time_point<std::chrono::system_clock> Time::timeOfFrameStart;

Time::Time ()
{
}

Time::~Time ()
{
}

// get duration of currently rendered frame
float Time::GetFrameDuration ()
{
	return frameDuration.count ();
}

// get time since start of current frame
float Time::GetTimeSinceFrameStart ()
{
	timeSinceFrameStart = std::chrono::system_clock::now () - timeOfFrameStart;
	return timeSinceFrameStart.count ();
}

// reset tracked frame durations on new frame
void Time::Tick () 
{
	auto timeOfPreviousFrameStart = timeOfFrameStart;
	timeOfFrameStart = std::chrono::system_clock::now ();
	frameDuration = timeOfFrameStart - timeOfPreviousFrameStart;
}