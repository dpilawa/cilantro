#ifndef _TIME_H_
#define _TIME_H_

#include <chrono>

class Time
{
public:
	Time ();
	~Time ();

	// get duration of currently rendered frame
	static float GetFrameDeltaTime ();
	// reset tracked frame durations on new frame
	static void Tick ();

private:
	// duration of current frame
	static std::chrono::duration<float> frameDeltaTime;
	// start time of current frame (set on Tick)
	static std::chrono::high_resolution_clock::time_point timeOfFrameStart;
};

#endif