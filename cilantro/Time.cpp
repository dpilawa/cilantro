#include "Time.h"

std::chrono::duration<float> Time::frameDeltaTime;
std::chrono::duration<float> Time::frameRenderTime;
std::chrono::high_resolution_clock::time_point Time::timeOfFrameStart;
std::chrono::high_resolution_clock::time_point Time::timeOfFrameEnd;
std::chrono::high_resolution_clock::time_point Time::splitTime;

Time::Time ()
{
	ResetSplitTime ();
}

Time::~Time ()
{
}

float Time::GetFrameDeltaTime ()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(frameDeltaTime).count () / 1000000.0f;
}

float Time::GetFrameRenderTime ()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(frameRenderTime).count () / 1000000.0f;
}

void Time::ResetSplitTime ()
{
	splitTime = std::chrono::high_resolution_clock::now ();
}

float Time::GetTimeSinceSplitTime ()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now () - splitTime).count () / 1000000.0f;
}

void Time::Tick () 
{
	auto timeOfPreviousFrameStart = timeOfFrameStart;
	timeOfFrameStart = std::chrono::high_resolution_clock::now ();
	frameDeltaTime = timeOfFrameStart - timeOfPreviousFrameStart;
}

void Time::Tock ()
{
	timeOfFrameEnd = std::chrono::high_resolution_clock::now ();
	frameRenderTime = timeOfFrameEnd - timeOfFrameStart;
}
