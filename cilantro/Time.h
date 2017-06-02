#ifndef _TIME_H_
#define _TIME_H_

#include <chrono>

class Time
{
public:
	Time ();
	~Time ();

	// get duration of currently rendered frame (in seconds)
	static double GetFrameDeltaTime ();
	// reset tracked frame durations on new frame
	static void Tick ();

private:
	// duration of current frame
	static std::chrono::duration<double> frameDeltaTime;
	// start time of current frame (set on Tick)
	static std::chrono::steady_clock::time_point timeOfFrameStart;
};

#endif