#include "Chronos.h"
#include "ImGuiManager.h"

long long Chronos::frameCount_ = 0;
std::chrono::high_resolution_clock::time_point Chronos::lastTime_ = {};
std::chrono::high_resolution_clock::time_point Chronos::currentTime_ = {};
long long Chronos::fps_;

void Chronos::Update() {
	frameCount_++;

	currentTime_ = std::chrono::high_resolution_clock::now();
	auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(currentTime_ - lastTime_);

	if (elapsed_time.count() >= 1.0) {
		fps_ = frameCount_ / elapsed_time.count();

		// 次の計測のためにリセット
		frameCount_ = 0;
		lastTime_ = currentTime_;
	}
#ifdef _DEBUG
	/*ImGui::Begin("FPS");
	ImGui::Text("FPS: %lld", fps_);
	ImGui::End();*/
#endif // DEBUG
}