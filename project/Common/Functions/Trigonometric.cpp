#include "Trigonometric.h"

float cot(float theta) {
	float ret = 1.0f / std::tan(theta);
	return ret;
}

float Deg2Rad(float deg) {
	float ret = static_cast<float>(deg) * (3.14159265358979323846f / 180.0f);
	return ret;
}

float Rad2Deg(float rad) {
	float ret = static_cast<float>(rad) * (180.0f / 3.14159265358979323846f);
	return ret;
}

void remove_extension_in_place(std::string& filename) {
	// 最後の位置を特定
	size_t last_dot_pos = filename.find_last_of('.');

	if (last_dot_pos != std::string::npos) {
		// last_dot_pos の位置から文字列の最後まで削除する
		filename.erase(last_dot_pos);
	}
	// ドットが見つからなければ何もしない
}