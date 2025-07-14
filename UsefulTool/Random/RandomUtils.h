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
	static void Initialize();
	static float GetRandom(int num);
private:
	static float randomNum_;
};

class HighRandom {
public:
    HighRandom() : gen(std::random_device{}()) {}

    int GetInt(int min, int max);

    double GetFloat(double min, double max);

	double GetGauss(double mean, double stddev);

private:
    std::mt19937 gen;
};

class RandomUtils {
public:
	
private:
};



