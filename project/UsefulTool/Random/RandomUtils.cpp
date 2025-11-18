#include "RandomUtils.h"

std::unique_ptr<RandomUtils> RandomUtils::instance_ = nullptr;

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

float HighRandom::GetFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

double HighRandom::GetGauss(double mean, double stddev) {
    std::normal_distribution<> dist(mean, stddev);
    return dist(gen);
}

void RandomUtils::Initialize() {
    timeRandom_.Initialize();
}