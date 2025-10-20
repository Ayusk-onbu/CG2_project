#pragma once
#include <chrono>

class Chronos
{
public:
	static void Update();
private:
	static long long frameCount_;
	static std::chrono::high_resolution_clock::time_point lastTime_;
	static std::chrono::high_resolution_clock::time_point currentTime_;
	static long long fps_;
};

