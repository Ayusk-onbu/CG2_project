#include "TimedCall.h"

TimedCall::TimedCall(std::function<void()>f, uint32_t time)
	: f_(f), time_(time) {}

void TimedCall::UpDate() {
	if (time_-- > 0) {
		return;
	}
	if (time_ <= 0) {
		isFinish_ = true;
		// コールバック関数呼び出し
		f_;
	}
}