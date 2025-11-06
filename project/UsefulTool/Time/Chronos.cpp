#include "Chronos.h"
#include "ImGuiManager.h"
#include "Log.h"
#include <iostream>
#include <thread>

#pragma comment(lib,"winmm.lib")

std::unique_ptr<Chronos> Chronos::instance_ = nullptr;

void Chronos::SetTargetFPS(float targetFPS) {
	if (targetFPS > 0.0f) {
		targetFPS_ = targetFPS;

		long long microsec = static_cast<long long>(1000000.0f / targetFPS_);
		kMinTime_ = std::chrono::microseconds(microsec);
		microsec = static_cast<long long>(1000000.0f / (targetFPS_ + 5.0f));
		kMinCheckTime_ = std::chrono::microseconds(microsec);
		isFixedFPS_ = true;
		std::cout << "TargetFPS set :" << targetFPS_ << "(" << microsec << "microseconds per frame)" << std::endl;
	}
	else {
		isFixedFPS_ = false;
		std::cout << "FPS fixed rate disabled." << std::endl;
	}
}

void Chronos::Initialize()
{
	timeBeginPeriod(1);
	// FPS計測用の初期化
	lastTime_ = std::chrono::high_resolution_clock::now();
	frameCount_ = 0;
	fps_ = 0;

	// FPS固定用の初期化
	referenceTime_ = std::chrono::steady_clock::now();
	SetTargetFPS(60.0f);
	Log::ViewFile("Chronos Initialized");
}

void Chronos::Update() {
	FixedUpdate();
	CalculateFPS();
}

void Chronos::CalculateFPS() {
	frameCount_++;

	std::chrono::high_resolution_clock::time_point currentTime_ = std::chrono::high_resolution_clock::now();
	auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(currentTime_ - lastTime_);

	if (elapsed_time.count() >= 1.0) {
		fps_ = frameCount_ / elapsed_time.count();

		// 次の計測のためにリセット
		frameCount_ = 0;
		lastTime_ = currentTime_;
	}
}

void Chronos::FixedUpdate() {
	if (!isFixedFPS_) {
		return;
	}
	// 今の時間を入手
	auto now = std::chrono::steady_clock::now();

	// １Frameにかかった時間を計算
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - referenceTime_);

	// 待つ
	if (elapsed < kMinCheckTime_) {
		while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - referenceTime_) < kMinTime_) {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	referenceTime_ = std::chrono::steady_clock::now();
}

void Chronos::ChangeIsFixed() {
	isFixedFPS_ = isFixedFPS_ == true ? false : true;
}