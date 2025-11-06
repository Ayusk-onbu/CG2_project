#pragma once
#include <vector>
#include <chrono>

class Chronos
{
public:
	static Chronos* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<Chronos>();
		}
		return instance_.get();
	}
	static std::unique_ptr<Chronos> instance_;
public:
	Chronos() = default;
	~Chronos() = default;
public:
	void Initialize();// 中身コンストラクタでもよくね？
	void Update();
	void CalculateFPS();
	void FixedUpdate();
	void ChangeIsFixed();
	bool GetIsFixed() { return isFixedFPS_; }
	void SetTargetFPS(float targetFPS);
	long long GetFPS() { return fps_; }
private:
	/*+*+*+*+*+*+*+*+*+*+*+*
	**    FPS計測用
	*+*+*+*+*+*+*+*+*+*+*+*/
	long long frameCount_;
	std::chrono::high_resolution_clock::time_point lastTime_;
	long long fps_;

	/*+*+*+*+*+*+*+*+*+*+*+*
	**    FPS固定用
	*+*+*+*+*+*+*+*+*+*+*+*/
	std::chrono::steady_clock::time_point referenceTime_;
	float targetFPS_ = 60.0f;
	std::chrono::microseconds kMinTime_;
	std::chrono::microseconds kMinCheckTime_;

	bool isFixedFPS_ = true;
};

