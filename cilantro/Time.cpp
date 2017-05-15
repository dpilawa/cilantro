#include "Time.h"

Time::Time ()
{
}

Time::~Time ()
{
}

// get time duration of currently rendered frame
float Time::GetFrameDuration ()
{
	return frameDuration.count ();
}

// get time since last frame
float Time::TimeSinceLastFrame ()
{
	return (std::chrono::high_resolution_clock::now () - thisFrameTime).count ();
}

// reset tracked frame durations on new frame
void Time::Tick () 
{
	std::chrono::high_resolution_clock::time_point prevFrameTime;
	thisFrameTime = std::chrono::high_resolution_clock::now ();
	frameDuration = thisFrameTime - prevFrameTime;
}