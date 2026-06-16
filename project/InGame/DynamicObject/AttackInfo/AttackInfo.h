#pragma once
#include <string>
#include "KeyFrame.h"

// アタック系の基底クラス
struct BaseAttackData {
	std::string name;// My name

	float timer;// 経過時間
	float duration;// この攻撃の時間

	float recoveryTime;// 硬直時間(攻撃終了から)
	float cancelAcceptTime;
};

struct MeleeAttackData : public BaseAttackData{
	float damage;// 魔法は弾の方に持たせるため基底には持たせない
	// 当たり判定は武器に持たせるから別にいい
};