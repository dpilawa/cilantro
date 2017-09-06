#ifndef _TIME_H_
#define _TIME_H_

#include "cilantroengine.h"
#include <chrono>

class Time
{
public:
	Time ();
	~Time ();

	// return delta-time of currently rendered frame (in seconds)
	static __EAPI float GetFrameDeltaTime ();
	// return rendering time of last frame (in seconds)
	static __EAPI float GetFrameRenderTime ();
	// calculate delta-time and reset begin timestamps on new frame
	static void Tick ();
	// set end timestamp and calculate frame rendering time
	static void Tock ();

private:
	// delta-time of current frame
	static std::chrono::duration<float> frameDeltaTime;
	// rendering time of current frame
	static std::chrono::duration<float> frameRenderTime;
	// start time of current frame (set on Tick)
	static std::chrono::high_resolution_clock::time_point timeOfFrameStart;
	// end time of current frame (set on Tock)
	static std::chrono::high_resolution_clock::time_point timeOfFrameEnd;
};

#endif