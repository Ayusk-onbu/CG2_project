#pragma once
#include "../../../AttackInfo/AttackInfo.h"
#include <vector>

struct ActionData {
	std::string name;// My name

	float timer;// actionの経過時間
	float duration;// このactionの時間

	std::string attackDataID;// 使用する攻撃のname
	std::string motionName;// 使用するモーションの名前
	std::vector<KeyframeString> events;// 発火するタイミングとID(パーティクルとかエフェクトとか)
	KeyframeString cameraWorkID;// 開始のタイミングとID
};