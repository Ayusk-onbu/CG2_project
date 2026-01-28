#pragma once
#include <memory>

class PauseSystem final {
public:
	static PauseSystem* GetInstance() {
		if (instance_ == nullptr) {
			instance_ = std::make_unique<PauseSystem>();
		}
		return instance_.get();
	}
public:
	void Initialize();
	bool Update(bool canPause);
	void Draw();
public:
	bool GetIsPause()const { return pause_; }
	void SetPause(bool pause) { pause_ = pause; }
private:
	static std::unique_ptr<PauseSystem>instance_;
	bool pause_ = false;
};