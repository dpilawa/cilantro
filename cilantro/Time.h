#ifndef _TIME_H_
#define _TIME_H_

#include <chrono>

class Time
{
	// time duration for current frame
	static std::chrono::duration<float> frameDuration;
	// time stamp of current frame (set on Tick)
	static std::chrono::high_resolution_clock::time_point thisFrameTime;

public:
	Time ();
	~Time ();

	// get time duration of currently rendered frame
	static float GetFrameDuration ();
	// get time since last frame
	static float TimeSinceLastFrame ();
	// reset tracked frame durations on new frame
	static void Tick ();
};

#endif