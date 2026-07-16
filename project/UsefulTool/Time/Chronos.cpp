#include "Chronos.h"
#include "ImGuiManager.h"
#include "EventManager.h"
#include "Log.h"
#include <iostream>
#include <thread>

#pragma comment(lib,"winmm.lib")
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

	previousFrameTime_ = std::chrono::high_resolution_clock::now();
	deltaTime_ = 1.0f / 60.0f;

	startTime_ = std::chrono::high_resolution_clock::now();
	totalTime_ = 0.0f;
	gameTime_ = 0.0;
	timeScale_ = 1.0f;

	// FPS固定用の初期化
	referenceTime_ = std::chrono::steady_clock::now();
	SetTargetFPS(60.0f);
	Log::ViewFile("Chronos Initialized\n");
}

void Chronos::Update() {
	auto currentFrameTime = std::chrono::high_resolution_clock::now();
	
	std::chrono::duration<float> elapsed = currentFrameTime - previousFrameTime_;
	deltaTime_ = elapsed.count();
	previousFrameTime_ = currentFrameTime; // 次のフレームのために現在の時間を保存

	std::chrono::duration<float> elapsedTotal = currentFrameTime - startTime_;
	totalTime_ = elapsedTotal.count();

	if (!isPaused_) {
		gameTime_ += static_cast<double>(deltaTime_ * timeScale_);
	}

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

	// 今回のフレームの目標時刻
	auto targetTime = referenceTime_ + kMinTime_;
	auto now = std::chrono::steady_clock::now();

	if (now < targetTime) {
		// スリープの誤差を考慮し、目標時刻の2ミリ秒前まではSleepで待機（CPU使用率削減）
		auto sleepTime = targetTime - now - std::chrono::milliseconds(2);
		if (sleepTime > std::chrono::microseconds(0)) {
			std::this_thread::sleep_for(sleepTime);
		}

		// 残りのわずかな時間はスピンロック（空ループ）で1マイクロ秒単位で正確に待つ
		while (std::chrono::steady_clock::now() < targetTime) {
			// 何もしない（ビジーウェイト）
		}
	}

	// 基準時間を更新
	// ※現在時刻で上書きすると微小なズレが蓄積するため、理想は targetTime を次の基準にする
	referenceTime_ = std::chrono::steady_clock::now(); 
}

void Chronos::ChangeIsFixed() {
	isFixedFPS_ = isFixedFPS_ == true ? false : true;
}

void Timer::Initialize(float firstTime, float deadline, bool isLoop) {
	data_.initialTime = firstTime;
	data_.deadlineTime = deadline;
	data_.isLoop = isLoop;
	data_.timer = data_.initialTime;

	// 最後の時間が最初の時間より小さいなら 5 -> 0なのでカウントダウン。
	isDown_ = data_.deadlineTime < data_.initialTime ? true : false;

	hasTriggered_ = false;
	timeSinceEnd_ = 0.0f;
}

void Timer::Update(float deltaTime) {
	// もしカウントダウンなら
	if (isDown_) {
		deltaTime *= -1.0f;
	}

	data_.timer += deltaTime;

	if (isDown_) {
		if (data_.timer <= data_.deadlineTime) {
			if (data_.isLoop) {
				data_.timer = data_.initialTime;
				hasTriggered_ = true;
				return;
			}
		}
	}
	else {
		if (data_.timer >= data_.deadlineTime) {
			if (data_.isLoop) {
				data_.timer = data_.initialTime;
				hasTriggered_ = true;
				return;
			}
		}
	}
	hasTriggered_ = false;
}

bool Timer::IsEnd() const{
	// ループ用
	if (data_.isLoop) {
		return hasTriggered_;
	}
	
	if (isDown_) {
		if (data_.timer <= data_.deadlineTime) {
			return true;
		}
	}
	else {
		if (data_.timer >= data_.deadlineTime) {
			return true;
		}
	}
	return false;
}

int TimeKeeper::RegisterTimer(float firstTime, float deadline, bool isLoop) {
	Timer newTimer;
	newTimer.Initialize(firstTime, deadline, isLoop);

	int checkKey = 0;
	// 自動でつかっていないところにいれる
	while (timerBox_.find(checkKey) != timerBox_.end()) {
		checkKey++; // 登録済みの場合は +1 して次を探す
	}

	timerBox_[checkKey] = newTimer;
	return checkKey;
}

void TimeKeeper::RemoveTimer(int key) {
	timerBox_.erase(key);
	Log::View(("Erase Timer Key : " + std::to_string(key)));
}

bool TimeKeeper::IsEnd(int key) {
	auto it = timerBox_.find(key);
	if (it != timerBox_.end()) {
		return it->second.IsEnd();
	}
	// 存在しない：けされた：かもなのでTRUE
	Log::View(("Not Found Timer Key : " + std::to_string(key)));
	return true;
}

void TimeKeeper::Update(float deltaTime) {
	// unordered_map をイテレートしながら更新
	// ※ ループ内で要素を削除(erase)する可能性があるため、イテレータの進め方に注意します
	for (auto it = timerBox_.begin(); it != timerBox_.end(); ) {

		it->second.Update(deltaTime);

		if (!it->second.IsLoop() && it->second.IsEnd()) {
			// ループでは無いなら
			it->second.AddTimeSinceEnd(deltaTime);

			// 終了してから10秒以上放置されたら、用済みとみなして自動削除
			if (it->second.GetTimeSinceEnd() > 10.0f) {
				Log::View("10 second Over since IsEnd: key " + std::to_string(it->first));
				it = timerBox_.erase(it);
				continue; // 次の要素へ
			}
		}
		// タイマーが完了していない、またはループする場合はそのまま次へ
		++it;
	}
}