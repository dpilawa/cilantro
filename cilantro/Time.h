#ifndef _TIME_H_
#define _TIME_H_

#include <chrono>

class Time
{
	// duration of current frame
	static std::chrono::duration<float> frameDuration;
	// time elapsed since start of current frame
	static std::chrono::duration<float> timeSinceFrameStart;
	// start time of current frame (set on Tick)
	static std::chrono::time_point<std::chrono::system_clock> timeOfFrameStart;

public:
	Time ();
	~Time ();

	// get duration of currently rendered frame
	static float GetFrameDuration ();
	// get time since start of current frame
	static float GetTimeSinceFrameStart ();
	// reset tracked frame durations on new frame
	static void Tick ();
};

#endif