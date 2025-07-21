#pragma once
#include <functional>

class TimedCall
{
public:
	TimedCall(std::function<void()>f, uint32_t time);

	void UpDate();
	bool IsFinished() { return isFinish_; }
private:
	std::function<void()>f_;
	uint32_t time_;
	bool isFinish_ = false;
};

