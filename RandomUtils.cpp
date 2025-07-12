#include "RandomUtils.h"

float TimeRandom::randomNum_ = 0;

void TimeRandom::Initialize() {
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

float TimeRandom::GetRandom(int num) {
	randomNum_ = static_cast<float>(std::rand() % num);
	return randomNum_;
}

int HighRandom::GetInt(int min, int max) {
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

double HighRandom::GetFloat(double min, double max) {
    std::uniform_real_distribution<> dist(min, max);
    return dist(gen);
}

double HighRandom::GetGauss(double mean, double stddev) {
    std::normal_distribution<> dist(mean, stddev);
    return dist(gen);
}