#pragma once
#include <vector>
#include <chrono>
#include "../ISingleton.h"


//
// 【 FPS等を測る用のクラス 】
//
class Chronos :
	public ISingleton<Chronos>
{
public:
	friend class ISingleton<Chronos>;
public:
	Chronos() = default;
	~Chronos() = default;
public:
	void Initialize();
	void Update();
public:
	void ChangeIsFixed();
	bool GetIsFixed() { return isFixedFPS_; }
	void SetTargetFPS(float targetFPS);
	long long GetFPS() { return fps_; }
	float GetDeltaTime() const { return deltaTime_; }
private:
	void CalculateFPS();
	void FixedUpdate();
private:
	/*+*+*+*+*+*+*+*+*+*+*+*
	**    FPS計測用
	*+*+*+*+*+*+*+*+*+*+*+*/
	long long frameCount_;
	std::chrono::high_resolution_clock::time_point lastTime_;
	long long fps_;

	std::chrono::high_resolution_clock::time_point previousFrameTime_;
	float deltaTime_ = 0.0f;
	/*+*+*+*+*+*+*+*+*+*+*+*
	**    FPS固定用
	*+*+*+*+*+*+*+*+*+*+*+*/
	std::chrono::steady_clock::time_point referenceTime_;
	float targetFPS_ = 60.0f;
	std::chrono::microseconds kMinTime_;
	std::chrono::microseconds kMinCheckTime_;

	bool isFixedFPS_ = true;
};


struct TimerData {
	float timer;// 今の時間
	float deadlineTime;// 目標の時間
	float initialTime;// 最初の時間
	bool isLoop;// ループするかどうか
};

class Timer {
public:
	void Initialize(const TimerData& data);
	bool Update(float deltaTime);

private:
	TimerData data_;
};

class TimeKeeper : public ISingleton<TimeKeeper>
{
public:
    friend class ISingleton<TimeKeeper>;
	TimeKeeper() = default;
	~TimeKeeper() = default;
public:
    // 毎フレーム Chronos 等から 経過時間(deltaTime) を受け取って更新する
    void Update(float deltaTime) {
        for (auto it = timers_.begin(); it != timers_.end(); ) {
            it->timer -= deltaTime;

            //// 時間がゼロ以下になったら発火
            //if (it->timer <= 0.0f) {
            //    if (it->action) {
            //        it->action(); // 登録された EventManager の発火などを実行
            //    }

            //    if (it->isLoop) {
            //        // ループ設定なら時間をリセットして次へ
            //        it->timeRemaining += it->initialTime;
            //        ++it;
            //    }
            //    else {
            //        // ループしないならリストから削除
            //        it = timers_.erase(it);
            //    }
            //}
            //else {
            //    ++it;
            //}
        }
    }

private:
	

    std::vector<TimerData> timers_;
};