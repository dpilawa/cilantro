#include "Time.h"

std::chrono::duration<double> Time::frameDeltaTime;
std::chrono::steady_clock::time_point Time::timeOfFrameStart;

Time::Time ()
{
}

Time::~Time ()
{
}


double Time::GetFrameDeltaTime ()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(frameDeltaTime).count () / 1000000.0f;
}


void Time::Tick () 
{
	auto timeOfPreviousFrameStart = timeOfFrameStart;
	timeOfFrameStart = std::chrono::steady_clock::now ();
	frameDeltaTime = timeOfFrameStart - timeOfPreviousFrameStart;
}