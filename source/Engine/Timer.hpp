#pragma once

#include<chrono>
#include<string>
#include<sstream>

class Timer
{
	using msec = std::chrono::milliseconds;
	template<typename T> using duration = std::chrono::duration<T>;
	using steady_clock = std::chrono::steady_clock;
	using time_point = std::chrono::time_point<steady_clock>;

	static constexpr float s_tickPerSecond = 1000.0f;
	static constexpr float s_SecondsCount = 1.0f;
public:
	Timer() 
		: 
		time{ 0.0f },
		previousTime{ 0.0f },
		deltaTime(0.0f),
		frames{ 0 },
		framesPerSecond{ 0 }
	{ 
		Reset(); 
	}
	int GetFPS() const { return framesPerSecond; }
	float GetTime() const { return time; }
	float GetPrevTime() const { return previousTime; }
	void Update();
	void Reset();
private:
	void IntervalStart();
	float IntervalEnd();
private:
	time_point time_point_start;
	time_point time_point_end;
	duration<float> d;
	msec ms;
	unsigned int frames;			// Счетчик кадров
	unsigned int framesPerSecond;	// Сколько кадров прошло за одну секунду
	float time;						// Текущее время в секундах
	float previousTime;				// Время с прошлого кадра
	float deltaTime;				// Сколько времени прошло с прошлого кадра
};


