#pragma once
#include <iostream>
#include <cstdint>
#include <ctime>
#include <random>

enum class RANDOMTYPE {
	TimeRandom,
};

class TimeRandom {
public:
	void Initialize();
	float GetRandom(int num);
private:
	float randomNum_{};
};

class HighRandom {
public:
    HighRandom() : gen(std::random_device{}()) {}

    int GetInt(int min, int max);

    float GetFloat(float min, float max);

	double GetGauss(double mean, double stddev);

private:
    std::mt19937 gen;
};

class RandomUtils {
public:
	static RandomUtils* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<RandomUtils>();
		}
		return instance_.get();
	}
public:
	void Initialize();
	TimeRandom& GetTimeRandom() { return timeRandom_; }
	HighRandom& GetHighRandom() { return highRandom_; }
private:
	static std::unique_ptr<RandomUtils>instance_;

	TimeRandom timeRandom_;
	HighRandom highRandom_;
};



