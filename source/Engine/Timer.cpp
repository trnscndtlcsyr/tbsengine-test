#include "Timer.hpp"

void Timer::Update()
{
    deltaTime = IntervalEnd();
    time_point_start = time_point_end;
    time += deltaTime / s_tickPerSecond;
    frames++;
    if (time - previousTime >= s_SecondsCount)
    {
        framesPerSecond = frames;
        frames = 0;
        previousTime = time;
    }
}

void Timer::Reset()
{
    IntervalStart();
    previousTime = time;
}

void Timer::IntervalStart()
{
    time_point_start = std::chrono::steady_clock::now();
}

float Timer::IntervalEnd()
{
	time_point_end = std::chrono::steady_clock::now();
	d = time_point_end - time_point_start;
    ms = std::chrono::duration_cast<msec>(d);
	return ms.count();
}

