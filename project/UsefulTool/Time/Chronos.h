#pragma once
#include <vector>
#include <chrono>
#include <unordered_map>
#include "../ISingleton.h"

struct PerFrame {

};

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
	float GetTotalTime() const { return totalTime_; }
	float GetGameTime() const { return static_cast<float>(gameTime_); }
	// スローモーションや倍速の制御（1.0で通常、0.5で半分の速度）
	void SetTimeScale(float scale) { timeScale_ = scale; }
	float GetTimeScale() const { return timeScale_; }
	void SetPause(bool isPause) { isPaused_ = isPause; }
	bool GetIsPaused() const { return isPaused_; }

	// ゲーム用のDeltaTime（スローやポーズが反映されたもの）
	float GetGameDeltaTime() const {
		return isPaused_ ? 0.0f : deltaTime_ * timeScale_;
	}
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
	std::chrono::high_resolution_clock::time_point startTime_; // ゲーム開始時間
	float totalTime_ = 0.0f;                                   // 総経過時間
	// ゲーム内時間（※長時間プレイでのfloatの桁落ちを防ぐため、ここだけ double にしておくと完璧です！）
	double gameTime_ = 0.0;
	float timeScale_ = 1.0f;
	bool isPaused_ = false;
};


struct TimerData {
	float timer;// 今の時間
	float deadlineTime;// 目標の時間
	float initialTime;// 最初の時間
	bool isLoop;// ループするかどうか
};

class Timer {
public:
	/// <summary>
	/// 代わりに時間を測る装置の初期化
	/// </summary>
	/// <param name="firstTime">最初の時間</param>
	/// <param name="deadline">終わりの時間</param>
	/// <param name="isLoop">ループするかどうか</param>
	void Initialize(float firstTime, float deadline = 0.0f, bool isLoop = false);
	/// <summary>
	/// 時間を経過させる処理
	/// </summary>
	/// <param name="deltaTime">進める時間の量</param>
	/// <returns>目標の時間に達成したか</returns>
	void Update(float deltaTime);

	// ループするかどうか
	bool IsLoop() const { return data_.isLoop; }

	bool IsEnd()const;

	void AddTimeSinceEnd(float deltaTime) { timeSinceEnd_ += deltaTime; }
	float GetTimeSinceEnd() const { return timeSinceEnd_; }
private:
	TimerData data_;
	// カウントダウンかどうか
	bool isDown_;
	// ループの場合にちょうど終わったかどうかを判定する
	bool hasTriggered_ = false;
	// 終わってからの放置時間(自動で廃棄するため)
	float timeSinceEnd_ = 0.0f;
};

class TimeKeeper : public ISingleton<TimeKeeper>
{
public:
    friend class ISingleton<TimeKeeper>;
	TimeKeeper() = default;
	~TimeKeeper() = default;
public:
    // 毎フレーム Chronos 等から 経過時間(deltaTime) を受け取って更新する
	void Update(float deltaTime);
	// ※ key に GAMEEVENTID をキャストして渡すと、完了時に自動で EventManager が発火します
	int RegisterTimer(float firstTime, float deadline = 0.0f, bool isLoop = false);

	// 使わなくなったら席を空ける
	void RemoveTimer(int key);

	bool IsEnd(int key);
private:
	std::unordered_map<int, Timer>timerBox_;
};